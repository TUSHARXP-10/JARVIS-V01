#pragma once

#include <atomic>
#include <cstdint>
#include <vector>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace jarvis {

// Forward declarations
class WakeWordDetector;
class SpeechRecognizer;
class TextToSpeech;
class AudioCapture;

enum class PipelineState {
    IDLE,
    LISTENING,
    THINKING,
    SPEAKING,
    ERROR
};

// Lock-free ring buffer for audio data
class AudioRingBuffer {
public:
    AudioRingBuffer(size_t capacity);
    ~AudioRingBuffer();

    bool write(const int16_t* data, size_t count);
    size_t read(int16_t* buffer, size_t count);
    size_t available() const;
    size_t freeSpace() const;
    void clear();

private:
    std::vector<int16_t> buffer_;
    std::atomic<size_t> head_{0};
    std::atomic<size_t> tail_{0};
    const size_t capacity_;
};

// Audio resampler for format normalization
class AudioResampler {
public:
    AudioResampler(int inputRate, int outputRate, int channels = 1);
    ~AudioResampler();

    std::vector<int16_t> resample(const int16_t* input, size_t inputFrames);
    void reset();

private:
    int inputRate_;
    int outputRate_;
    int channels_;
    double ratio_;
    double accumulator_;
};

// Voice Activity Detection (VAD)
class VoiceActivityDetector {
public:
    VoiceActivityDetector(int sampleRate, int frameSize);
    ~VoiceActivityDetector();

    bool processFrame(const int16_t* frame, size_t frameSize);
    void setThreshold(float threshold);
    void setSilenceTimeout(int ms);

private:
    int sampleRate_;
    int frameSize_;
    float threshold_;
    int silenceTimeoutMs_;
    int silentFrames_;
    bool voiceDetected_;
};

// Audio pipeline manager
class AudioPipeline {
public:
    using WakeWordCallback = std::function<void()>;
    using SpeechCallback = std::function<void(const std::string&)>;
    using TTSCallback = std::function<void(const std::string&)>;

    AudioPipeline();
    ~AudioPipeline();

    bool initialize(int sampleRate, int channels, int frameSize);
    void start();
    void stop();
    void setWakeWordCallback(WakeWordCallback callback);
    void setSpeechCallback(SpeechCallback callback);
    void setTTSCallback(TTSCallback callback);

    PipelineState getState() const;
    void setState(PipelineState state);

    // Configuration
    void setWakeWordSensitivity(float sensitivity);
    void setSilenceTimeout(int ms);
    void setMaxUtteranceDuration(int ms);

    // Debug and metrics
    struct Metrics {
        double wakeToStartMs = 0.0;
        double speechDurationMs = 0.0;
        double sttLatencyMs = 0.0;
        double nluLatencyMs = 0.0;
        double ttsLatencyMs = 0.0;
        int falseWakes = 0;
    };

    Metrics getMetrics() const;

private:
    void audioCaptureLoop();
    void wakeWordLoop();
    void speechRecognitionLoop();
    void ttsLoop();

    void handleWakeWord();
    void handleSpeechEnd();
    void handleTTSComplete();

    // Core components
    std::unique_ptr<AudioCapture> audioCapture_;
    std::unique_ptr<WakeWordDetector> wakeWordDetector_;
    std::unique_ptr<SpeechRecognizer> speechRecognizer_;
    std::unique_ptr<TextToSpeech> textToSpeech_;

    // Audio buffers
    std::unique_ptr<AudioRingBuffer> rawAudioBuffer_;
    std::unique_ptr<AudioRingBuffer> wakeWordBuffer_;
    std::unique_ptr<AudioRingBuffer> sttBuffer_;
    std::unique_ptr<AudioResampler> wakeWordResampler_;
    std::unique_ptr<AudioResampler> sttResampler_;
    std::unique_ptr<VoiceActivityDetector> vad_;

    // Threading
    std::thread audioThread_;
    std::thread wakeWordThread_;
    std::thread sttThread_;
    std::thread ttsThread_;
    std::atomic<bool> running_{false};

    // State management
    std::atomic<PipelineState> state_{PipelineState::IDLE};
    std::mutex stateMutex_;
    std::condition_variable stateCV_;

    // Callbacks
    WakeWordCallback wakeWordCallback_;
    SpeechCallback speechCallback_;
    TTSCallback ttsCallback_;

    // Configuration
    int sampleRate_;
    int channels_;
    int frameSize_;
    float wakeWordSensitivity_;
    int silenceTimeoutMs_;
    int maxUtteranceDurationMs_;

    // Metrics
    mutable std::mutex metricsMutex_;
    Metrics metrics_;
    std::chrono::steady_clock::time_point wakeStartTime_;
    std::chrono::steady_clock::time_point speechStartTime_;
};

} // namespace jarvis