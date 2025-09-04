#pragma once

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>

// Forward declaration for eSpeak
namespace espeak {
    struct espeak_VOICE;
}

namespace jarvis {

/**
 * @brief Text-to-speech using eSpeak NG
 * 
 * This class provides text-to-speech functionality using
 * the eSpeak NG speech synthesis engine.
 */
class TextToSpeech {
public:
    TextToSpeech();
    ~TextToSpeech();

    /**
     * @brief Initialize the text-to-speech engine
     * @param voice Voice name (default: "en")
     * @param rate Speech rate in words per minute (default: 175)
     * @param volume Volume level [0-100] (default: 100)
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const std::string& voice = "en", 
                   int rate = 175, 
                   int volume = 100);

    /**
     * @brief Speak the given text
     * @param text Text to speak
     * @param async If true, speak asynchronously (default: true)
     */
    void speak(const std::string& text, bool async = true);

    /**
     * @brief Stop speaking immediately
     */
    void stop();

    /**
     * @brief Set speech rate
     * @param rate Speech rate in words per minute
     */
    void setRate(int rate);

    /**
     * @brief Set speech volume
     * @param volume Volume level [0-100]
     */
    void setVolume(int volume);

    /**
     * @brief Set speech voice
     * @param voice Voice name
     */
    void setVoice(const std::string& voice);

    /**
     * @brief Check if currently speaking
     * @return true if speaking, false otherwise
     */
    bool isSpeaking() const { return speaking_; }

    /**
     * @brief Check if TTS is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return initialized_; }

private:
    bool initialized_;
    std::atomic<bool> speaking_;
    std::atomic<bool> shouldStop_;
    std::thread speechThread_;
    std::mutex queueMutex_;
    std::queue<std::string> speechQueue_;
    std::atomic<bool> stopRequested_;

    void speechLoop();
    void speakInternal(const std::string& text);
    void initializeESpeak();
    void cleanupESpeak();
};

} // namespace jarvis