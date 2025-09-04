#include "speech/speech_recognizer.h"
#include "utils/logger.h"
#include <iostream>
#include <stdexcept>

#ifdef VOSK_FOUND
#include <vosk_api.h>
#endif

namespace jarvis {

class SpeechRecognizer::Impl {
public:
#ifdef VOSK_FOUND
    VoskModel* model = nullptr;
    VoskRecognizer* recognizer = nullptr;
#endif
    bool initialized = false;
    std::string modelPath;
    int sampleRate = 16000;
    bool partialResultsEnabled = true;
};

SpeechRecognizer::SpeechRecognizer() : impl_(std::make_unique<Impl>()) {}
SpeechRecognizer::~SpeechRecognizer() = default;

bool SpeechRecognizer::initialize(const std::string& modelPath, int sampleRate) {
    impl_->modelPath = modelPath;
    impl_->sampleRate = sampleRate;

#ifdef VOSK_FOUND
    if (modelPath.empty()) {
        LOG_ERROR("Vosk model path is empty");
        return false;
    }

    impl_->model = vosk_model_new(modelPath.c_str());
    if (!impl_->model) {
        LOG_ERROR("Failed to load Vosk model from: " << modelPath);
        return false;
    }

    impl_->recognizer = vosk_recognizer_new(impl_->model, sampleRate);
    if (!impl_->recognizer) {
        LOG_ERROR("Failed to create Vosk recognizer");
        vosk_model_free(impl_->model);
        impl_->model = nullptr;
        return false;
    }

    impl_->initialized = true;
    LOG_INFO("Speech recognizer initialized successfully with Vosk");
    return true;
#else
    LOG_WARNING("Vosk not available - using placeholder implementation");
    impl_->initialized = true;
    return true;
#endif
}

bool SpeechRecognizer::startRecognition() {
    if (!impl_->initialized) {
        LOG_ERROR("Speech recognizer not initialized");
        return false;
    }

#ifdef VOSK_FOUND
    if (!impl_->recognizer) {
        LOG_ERROR("Vosk recognizer not available");
        return false;
    }
#endif

    LOG_INFO("Speech recognition started");
    return true;
}

bool SpeechRecognizer::stopRecognition() {
    LOG_INFO("Speech recognition stopped");
    return true;
}

std::string SpeechRecognizer::processAudio(const std::vector<int16_t>& audioData) {
    if (!impl_->initialized) {
        LOG_ERROR("Speech recognizer not initialized");
        return "";
    }

#ifdef VOSK_FOUND
    if (!impl_->recognizer) {
        return "Vosk not available";
    }

    // Process audio data
    int result = vosk_recognizer_accept_waveform(impl_->recognizer, 
                                                 audioData.data(), 
                                                 audioData.size() * sizeof(int16_t));

    if (result == 0) {
        // Partial result
        if (impl_->partialResultsEnabled) {
            const char* partial = vosk_recognizer_partial_result(impl_->recognizer);
            if (partial && strlen(partial) > 2) {  // Skip empty results
                return std::string(partial);
            }
        }
        return "";
    } else if (result == 1) {
        // Final result
        const char* final = vosk_recognizer_final_result(impl_->recognizer);
        if (final) {
            return std::string(final);
        }
        return "";
    }
#else
    // Placeholder implementation
    static int counter = 0;
    if (++counter % 100 == 0) {
        return "{"text": "placeholder speech recognition result"}";
    }
    return "";
#endif
}

std::string SpeechRecognizer::getPartialResult() {
#ifdef VOSK_FOUND
    if (!impl_->recognizer) {
        return "";
    }

    const char* partial = vosk_recognizer_partial_result(impl_->recognizer);
    if (partial) {
        return std::string(partial);
    }
#endif
    return "";
}

std::string SpeechRecognizer::getFinalResult() {
#ifdef VOSK_FOUND
    if (!impl_->recognizer) {
        return "";
    }

    const char* final = vosk_recognizer_final_result(impl_->recognizer);
    if (final) {
        return std::string(final);
    }
#endif
    return "";
}

void SpeechRecognizer::reset() {
#ifdef VOSK_FOUND
    if (impl_->recognizer) {
        vosk_recognizer_reset(impl_->recognizer);
    }
#endif
}

void SpeechRecognizer::enablePartialResults(bool enable) {
    impl_->partialResultsEnabled = enable;
}

bool SpeechRecognizer::isInitialized() const {
    return impl_->initialized;
}

int SpeechRecognizer::getSampleRate() const {
    return impl_->sampleRate;
}

} // namespace jarvis