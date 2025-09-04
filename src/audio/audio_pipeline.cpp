#include "audio/audio_pipeline.h"
#include "audio/audio_capture.h"
#include "speech/wake_word_detector.h"
#include "speech/speech_recognizer.h"
#include "speech/text_to_speech.h"
#include "utils/logger.h"
#include <algorithm>
#include <cmath>
#include <chrono>

namespace jarvis {

// AudioRingBuffer implementation
AudioRingBuffer::AudioRingBuffer(size_t capacity)
    : capacity_(capacity + 1), buffer_(capacity + 1) {
}

AudioRingBuffer::~AudioRingBuffer() = default;

bool AudioRingBuffer::write(const int16_t* data, size_t count) {
    size_t free = freeSpace();
    if (count > free) {
        return false; // Buffer overflow
    }

    size_t tail = tail_.load(std::memory_order_relaxed);
    for (size_t i = 0; i < count; ++i) {
        buffer_[tail] = data[i];
        tail = (tail + 1) % capacity_;
    }
    tail_.store(tail, std::memory_order_release);
    return true;
}

size_t AudioRingBuffer::read(int16_t* buffer, size_t count) {
    size_t avail = available();
    size_t toRead = std::min(count, avail);
    
    size_t head = head_.load(std::memory_order_relaxed);
    for (size_t i = 0; i < toRead; ++i) {
        buffer[i] = buffer_[head];
        head = (head + 1) % capacity_;
    }
    head_.store(head, std::memory_order_release);
    return toRead;
}

size_t AudioRingBuffer::available() const {
    size_t head = head_.load(std::memory_order_acquire);
    size_t tail = tail_.load(std::memory_order_acquire);
    return (tail + capacity_ - head) % capacity_;
}

size_t AudioRingBuffer::freeSpace() const {
    return capacity_ - available() - 1;
}

void AudioRingBuffer::clear() {
    head_.store(0, std::memory_order_relaxed);
    tail_.store(0, std::memory_order_relaxed);
}

// AudioResampler implementation
AudioResampler::AudioResampler(int inputRate, int outputRate, int channels)
    : inputRate_(inputRate), outputRate_(outputRate), channels_(channels), 
      ratio_(static_cast<double>(outputRate) / inputRate), accumulator_(0.0) {
}

AudioResampler::~AudioResampler() = default;

std::vector<int16_t> AudioResampler::resample(const int16_t* input, size_t inputFrames) {
    if (inputRate_ == outputRate_) {
        return std::vector<int16_t>(input, input + inputFrames);
    }

    size_t outputFrames = static_cast<size_t>(inputFrames * ratio_);
    std::vector<int16_t> output(outputFrames);

    for (size_t i = 0; i < outputFrames; ++i) {
        double inputIndex = static_cast<double>(i) / ratio_;
        size_t index = static_cast<size_t>(inputIndex);
        double fraction = inputIndex - index;

        if (index + 1 < inputFrames) {
            // Linear interpolation
            output[i] = static_cast<int16_t>(
                input[index] * (1.0 - fraction) + input[index + 1] * fraction
            );
        } else {
            output[i] = input[index];
        }
    }

    return output;
}

void AudioResampler::reset() {
    accumulator_ = 0.0;
}

// VoiceActivityDetector implementation
VoiceActivityDetector::VoiceActivityDetector(int sampleRate, int frameSize)
    : sampleRate_(sampleRate), frameSize_(frameSize), threshold_(0.01f),
      silenceTimeoutMs_(2000), silentFrames_(0), voiceDetected_(false) {
}

VoiceActivityDetector::~VoiceActivityDetector() = default;

bool VoiceActivityDetector::processFrame(const int16_t* frame, size_t frameSize) {
    // Calculate RMS energy
    double sum = 0.0;
    for (size_t i = 0; i < frameSize; ++i) {
        double sample = static_cast<double>(frame[i]) / 32768.0;
        sum += sample * sample;
    }
    double rms = std::sqrt(sum / frameSize);

    if (rms > threshold_) {
        silentFrames_ = 0;
        voiceDetected_ = true;
        return true;
    } else {
        silentFrames_++;
        
        // Calculate silence duration
        double silenceDuration = (silentFrames_ * frameSize) / static_cast<double>(sampleRate_) * 1000.0;
        if (silenceDuration >= silenceTimeoutMs_) {
            voiceDetected_ = false;
            silentFrames_ = 0;
            return false;
        }
        return voiceDetected_;
    }
}

void VoiceActivityDetector::setThreshold(float threshold) {
    threshold_ = threshold;
}

void VoiceActivityDetector::setSilenceTimeout(int ms) {
    silenceTimeoutMs_ = ms;
}

// AudioPipeline implementation
AudioPipeline::AudioPipeline() = default;

AudioPipeline::~AudioPipeline() {
    stop();
}

bool AudioPipeline::initialize(int sampleRate, int channels, int frameSize) {
    sampleRate_ = sampleRate;
    channels_ = channels;
    frameSize_ = frameSize;
    
    // Initialize components
    audioCapture_ = std::make_unique<AudioCapture>();
    wakeWordDetector_ = std::make_unique<WakeWordDetector>();
    speechRecognizer_ = std::make_unique<SpeechRecognizer>();
    textToSpeech_ = std::make_unique<TextToSpeech>();

    // Initialize audio buffers
    rawAudioBuffer_ = std::make_unique<AudioRingBuffer>(sampleRate * 30); // 30 seconds buffer
    wakeWordBuffer_ = std::make_unique<AudioRingBuffer>(sampleRate * 5);    // 5 seconds buffer
    sttBuffer_ = std::make_unique<AudioRingBuffer>(sampleRate * 30);        // 30 seconds buffer

    // Initialize resamplers
    // Porcupine typically uses 16000 Hz
    wakeWordResampler_ = std::make_unique<AudioResampler>(sampleRate, 16000, channels);
    // Vosk uses 16000 Hz
    sttResampler_ = std::make_unique<AudioResampler>(sampleRate, 16000, channels);

    // Initialize VAD
    vad_ = std::make_unique<VoiceActivityDetector>(16000, frameSize);

    return true;
}

void AudioPipeline::start() {
    if (running_) return;
    
    running_ = true;
    state_ = PipelineState::IDLE;
    
    // Start threads
    audioThread_ = std::thread(&AudioPipeline::audioCaptureLoop, this);
    wakeWordThread_ = std::thread(&AudioPipeline::wakeWordLoop, this);
    sttThread_ = std::thread(&AudioPipeline::speechRecognitionLoop, this);
    ttsThread_ = std::thread(&AudioPipeline::ttsLoop, this);
}

void AudioPipeline::stop() {
    if (!running_) return;
    
    running_ = false;
    state_ = PipelineState::IDLE;
    
    // Notify all threads
    stateCV_.notify_all();
    
    // Join threads
    if (audioThread_.joinable()) audioThread_.join();
    if (wakeWordThread_.joinable()) wakeWordThread_.join();
    if (sttThread_.joinable()) sttThread_.join();
    if (ttsThread_.joinable()) ttsThread_.join();
}

void AudioPipeline::setWakeWordCallback(WakeWordCallback callback) {
    wakeWordCallback_ = callback;
}

void AudioPipeline::setSpeechCallback(SpeechCallback callback) {
    speechCallback_ = callback;
}

void AudioPipeline::setTTSCallback(TTSCallback callback) {
    ttsCallback_ = callback;
}

PipelineState AudioPipeline::getState() const {
    return state_.load(std::memory_order_acquire);
}

void AudioPipeline::setState(PipelineState state) {
    state_.store(state, std::memory_order_release);
    stateCV_.notify_all();
}

void AudioPipeline::setWakeWordSensitivity(float sensitivity) {
    wakeWordSensitivity_ = sensitivity;
}

void AudioPipeline::setSilenceTimeout(int ms) {
    silenceTimeoutMs_ = ms;
    if (vad_) {
        vad_->setSilenceTimeout(ms);
    }
}

void AudioPipeline::setMaxUtteranceDuration(int ms) {
    maxUtteranceDurationMs_ = ms;
}

AudioPipeline::Metrics AudioPipeline::getMetrics() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    return metrics_;
}

// Thread loops
void AudioPipeline::audioCaptureLoop() {
    LOG_INFO("Audio capture thread started");
    
    // This would integrate with AudioCapture
    while (running_) {
        // Simulate audio capture - in real implementation, this would come from AudioCapture
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        // For now, just simulate some audio data
        std::vector<int16_t> frame(frameSize_, 0);
        
        // Write to raw buffer
        rawAudioBuffer_->write(frame.data(), frame.size());
    }
    
    LOG_INFO("Audio capture thread stopped");
}

void AudioPipeline::wakeWordLoop() {
    LOG_INFO("Wake word detection thread started");
    
    const size_t porcupineFrameSize = 512; // Typical Porcupine frame size
    std::vector<int16_t> frame(porcupineFrameSize);
    
    while (running_) {
        if (getState() == PipelineState::IDLE) {
            // Read audio for wake word detection
            size_t available = rawAudioBuffer_->available();
            if (available >= porcupineFrameSize) {
                rawAudioBuffer_->read(frame.data(), porcupineFrameSize);
                
                // Resample to Porcupine format
                auto resampled = wakeWordResampler_->resample(frame.data(), porcupineFrameSize);
                
                // Process with Porcupine
                // This would integrate with WakeWordDetector
                bool detected = false; // Placeholder
                
                if (detected) {
                    handleWakeWord();
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    LOG_INFO("Wake word detection thread stopped");
}

void AudioPipeline::speechRecognitionLoop() {
    LOG_INFO("Speech recognition thread started");
    
    const size_t sttFrameSize = 4096; // Larger frame for STT
    std::vector<int16_t> frame(sttFrameSize);
    
    while (running_) {
        if (getState() == PipelineState::LISTENING) {
            size_t available = rawAudioBuffer_->available();
            if (available >= sttFrameSize) {
                rawAudioBuffer_->read(frame.data(), sttFrameSize);
                
                // Resample to Vosk format
                auto resampled = sttResampler_->resample(frame.data(), sttFrameSize);
                
                // Process with VAD and Vosk
                bool voiceActive = vad_->processFrame(resampled.data(), resampled.size());
                
                // This would integrate with SpeechRecognizer
                // Placeholder for Vosk processing
                
                if (!voiceActive) {
                    handleSpeechEnd();
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    LOG_INFO("Speech recognition thread stopped");
}

void AudioPipeline::ttsLoop() {
    LOG_INFO("TTS thread started");
    
    while (running_) {
        // This would integrate with TextToSpeech
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    LOG_INFO("TTS thread stopped");
}

// Event handlers
void AudioPipeline::handleWakeWord() {
    LOG_INFO("Wake word detected");
    
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metrics_.wakeToStartMs = 0.0; // Reset timing
        wakeStartTime_ = std::chrono::steady_clock::now();
    }
    
    if (wakeWordCallback_) {
        wakeWordCallback_();
    }
    
    setState(PipelineState::LISTENING);
    
    // Clear STT buffer for fresh utterance
    sttBuffer_->clear();
    vad_->reset();
}

void AudioPipeline::handleSpeechEnd() {
    LOG_INFO("Speech recognition complete");
    
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        auto endTime = std::chrono::steady_clock::now();
        metrics_.speechDurationMs = std::chrono::duration<double, std::milli>(
            endTime - speechStartTime_).count();
    }
    
    // This would integrate with SpeechRecognizer to get final result
    std::string transcript = "Simulated transcript";
    
    if (speechCallback_) {
        speechCallback_(transcript);
    }
    
    setState(PipelineState::THINKING);
    
    // Simulate NLU processing
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Simulate response generation
    std::string response = "I heard: " + transcript;
    
    setState(PipelineState::SPEAKING);
    
    if (ttsCallback_) {
        ttsCallback_(response);
    }
    
    // After TTS completes
    setState(PipelineState::IDLE);
}

void AudioPipeline::handleTTSComplete() {
    LOG_INFO("TTS playback complete");
    setState(PipelineState::IDLE);
}

} // namespace jarvis