#include "speech/wake_word_detector.h"
#include "utils/logger.h"
#include <iostream>
#include <stdexcept>

#ifdef PORCUPINE_FOUND
#include <pv_porcupine.h>
#endif

namespace jarvis {

class WakeWordDetector::AudioCapture {
public:
    AudioCapture() : stream_(nullptr) {}
    ~AudioCapture() {
        stopCapture();
    }
    
    bool initialize(int sampleRate, int channels) {
        sampleRate_ = sampleRate;
        channels_ = channels;
        
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            LOG_ERROR("PortAudio initialization failed: " << Pa_GetErrorText(err));
            return false;
        }
        
        err = Pa_OpenDefaultStream(&stream_, 
                                   channels, 
                                   0, 
                                   paInt16, 
                                   sampleRate, 
                                   512, 
                                   nullptr, 
                                   nullptr);
        
        if (err != paNoError) {
            LOG_ERROR("Failed to open audio stream: " << Pa_GetErrorText(err));
            return false;
        }
        
        return true;
    }
    
    void startCapture(std::function<void(const std::vector<int16_t>&)> callback) {
        if (!stream_) return;
        
        captureCallback_ = callback;
        shouldCapture_ = true;
        
        PaError err = Pa_StartStream(stream_);
        if (err != paNoError) {
            LOG_ERROR("Failed to start audio stream: " << Pa_GetErrorText(err));
            return;
        }
        
        captureThread_ = std::thread([this]() {
            while (shouldCapture_) {
                std::vector<int16_t> buffer(512);
                Pa_ReadStream(stream_, buffer.data(), buffer.size());
                if (captureCallback_) {
                    captureCallback_(buffer);
                }
            }
        });
    }
    
    void stopCapture() {
        shouldCapture_ = false;
        if (captureThread_.joinable()) {
            captureThread_.join();
        }
        
        if (stream_) {
            Pa_StopStream(stream_);
            Pa_CloseStream(stream_);
            stream_ = nullptr;
        }
        
        Pa_Terminate();
    }

private:
    PaStream* stream_;
    int sampleRate_;
    int channels_;
    std::thread captureThread_;
    std::atomic<bool> shouldCapture_{false};
    std::function<void(const std::vector<int16_t>&)> captureCallback_;
};

WakeWordDetector::WakeWordDetector() 
    : porcupine_(nullptr, [](pv_porcupine_t*) {})
    , audioCapture_(std::make_unique<AudioCapture>()) {}

WakeWordDetector::~WakeWordDetector() {
    stopDetection();
}

bool WakeWordDetector::initialize(const std::string& modelPath, 
                                const std::string& keywordPath, 
                                float sensitivity) {
#ifdef PORCUPINE_FOUND
    if (modelPath.empty() || keywordPath.empty()) {
        LOG_ERROR("Model path or keyword path is empty");
        return false;
    }

    pv_porcupine_t* porcupine = nullptr;
    const char* access_key = std::getenv("PICOVOICE_ACCESS_KEY");
    if (!access_key) {
        LOG_ERROR("PICOVOICE_ACCESS_KEY environment variable not set");
        return false;
    }

    pv_status_t status = pv_porcupine_init(
        access_key,
        modelPath.c_str(),
        1,
        &keywordPath.c_str(),
        &sensitivity,
        &porcupine
    );

    if (status != PV_STATUS_SUCCESS) {
        LOG_ERROR("Failed to initialize Porcupine: " << pv_status_to_string(status));
        return false;
    }

    porcupine_.reset(porcupine);
    LOG_INFO("Wake word detector initialized successfully with Porcupine");
    
    return true;
#else
    LOG_WARNING("Porcupine not available - using placeholder implementation");
    return true;
#endif
}

void WakeWordDetector::startDetection(WakeWordCallback callback) {
    if (running_) {
        LOG_WARNING("Wake word detection already running");
        return;
    }

    callback_ = callback;
    
    if (!audioCapture_->initialize(getSampleRate(), 1)) {
        LOG_ERROR("Failed to initialize audio capture");
        return;
    }

    running_ = true;
    shouldStop_ = false;
    
    detectionThread_ = std::thread(&WakeWordDetector::detectionLoop, this);
    LOG_INFO("Wake word detection started");
}

void WakeWordDetector::stopDetection() {
    if (!running_) return;

    shouldStop_ = true;
    running_ = false;
    
    if (detectionThread_.joinable()) {
        detectionThread_.join();
    }
    
    audioCapture_->stopCapture();
    LOG_INFO("Wake word detection stopped");
}

void WakeWordDetector::detectionLoop() {
    // Start audio capture with callback
    audioCapture_->startCapture([this](const std::vector<int16_t>& audioData) {
        // Accumulate audio data until we have a full frame
        static std::vector<int16_t> accumulatedAudio;
        accumulatedAudio.insert(accumulatedAudio.end(), audioData.begin(), audioData.end());
        
        int frameLength = getFrameLength();
        
        // Process complete frames
        while (accumulatedAudio.size() >= static_cast<size_t>(frameLength)) {
            std::vector<int16_t> frame(accumulatedAudio.begin(), accumulatedAudio.begin() + frameLength);
            accumulatedAudio.erase(accumulatedAudio.begin(), accumulatedAudio.begin() + frameLength);
            
            if (processAudioFrame(frame)) {
                if (callback_) {
                    callback_();
                }
            }
        }
    });
    
    // Wait until stop is requested
    while (!shouldStop_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

bool WakeWordDetector::processAudioFrame(const std::vector<int16_t>& frame) {
#ifdef PORCUPINE_FOUND
    if (!porcupine_) return false;
    
    int32_t keyword_index = -1;
    pv_status_t status = pv_porcupine_process(
        porcupine_.get(),
        frame.data(),
        &keyword_index
    );
    
    if (status != PV_STATUS_SUCCESS) {
        LOG_ERROR("Porcupine processing failed: " << pv_status_to_string(status));
        return false;
    }
    
    return keyword_index == 0;
#else
    // Placeholder implementation - randomly trigger for testing
    static int counter = 0;
    return (++counter % 100) == 0;
#endif
}

int WakeWordDetector::getSampleRate() const {
#ifdef PORCUPINE_FOUND
    return pv_sample_rate();
#else
    return 16000; // Default fallback
#endif
}

int WakeWordDetector::getFrameLength() const {
#ifdef PORCUPINE_FOUND
    return pv_porcupine_frame_length();
#else
    return 512; // Default fallback
#endif
}

} // namespace jarvis