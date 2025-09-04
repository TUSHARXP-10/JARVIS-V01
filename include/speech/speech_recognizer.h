#pragma once

#include <string>
#include <memory>
#include <vector>

// Forward declarations for Vosk
struct VoskModel;
typedef struct VoskModel VoskModel;
struct VoskRecognizer;
typedef struct VoskRecognizer VoskRecognizer;

namespace jarvis {

/**
 * @brief Speech recognition using Vosk API
 * 
 * This class provides speech-to-text functionality using
 * the Vosk offline speech recognition engine.
 */
class SpeechRecognizer {
public:
    SpeechRecognizer();
    ~SpeechRecognizer();

    /**
     * @brief Initialize the speech recognizer
     * @param modelPath Path to the Vosk model directory
     * @param sampleRate Audio sample rate in Hz (default: 16000)
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const std::string& modelPath, float sampleRate = 16000.0f);

    /**
     * @brief Start speech recognition
     * @return true if started successfully, false otherwise
     */
    bool startRecognition();

    /**
     * @brief Stop speech recognition
     */
    void stopRecognition();

    /**
     * @brief Process audio data for speech recognition
     * @param audioData Audio data as 16-bit PCM samples
     * @return Recognized text if available, empty string otherwise
     */
    std::string processAudio(const std::vector<int16_t>& audioData);

    /**
     * @brief Get the final recognition result
     * @return Final recognized text
     */
    std::string getFinalResult();

    /**
     * @brief Check if recognition is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return running_; }

    /**
     * @brief Check if the model is loaded
     * @return true if model is loaded, false otherwise
     */
    bool isModelLoaded() const { return model_ != nullptr; }

private:
    std::unique_ptr<VoskModel, void(*)(VoskModel*)> model_;
    std::unique_ptr<VoskRecognizer, void(*)(VoskRecognizer*)> recognizer_;
    bool running_;
    bool modelLoaded_;

    void cleanup();
};

} // namespace jarvis