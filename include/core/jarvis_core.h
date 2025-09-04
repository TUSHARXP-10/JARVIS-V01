#pragma once

#include <memory>
#include <string>
#include <thread>
#include <atomic>

namespace jarvis {

class WakeWordDetector;
class SpeechRecognizer;
class TextToSpeech;
class IntentParser;
class PluginManager;
class ConfigManager;
class Logger;

/**
 * @brief Main Jarvis voice assistant core class
 * 
 * This class orchestrates the entire voice assistant pipeline:
 * 1. Wake word detection
 * 2. Speech recognition
 * 3. Intent parsing
 * 4. Command execution
 * 5. Text-to-speech response
 */
class JarvisCore {
public:
    JarvisCore();
    ~JarvisCore();

    /**
     * @brief Initialize the Jarvis system
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Start the voice assistant
     */
    void start();

    /**
     * @brief Stop the voice assistant
     */
    void stop();

    /**
     * @brief Process a voice command
     * @param command The recognized text command
     */
    void processCommand(const std::string& command);

    /**
     * @brief Check if the system is running
     * @return true if running, false otherwise
     */
    bool isRunning() const { return running_; }

private:
    std::unique_ptr<WakeWordDetector> wakeWordDetector_;
    std::unique_ptr<SpeechRecognizer> speechRecognizer_;
    std::unique_ptr<TextToSpeech> textToSpeech_;
    std::unique_ptr<IntentParser> intentParser_;
    std::unique_ptr<PluginManager> pluginManager_;
    std::unique_ptr<ConfigManager> configManager_;
    std::unique_ptr<Logger> logger_;

    std::atomic<bool> running_{false};
    std::thread processingThread_;

    void processingLoop();
    void handleWakeWordDetected();
};

} // namespace jarvis