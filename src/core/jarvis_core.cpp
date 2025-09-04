#include "core/jarvis_core.h"
#include "speech/wake_word_detector.h"
#include "speech/speech_recognizer.h"
#include "speech/text_to_speech.h"
#include "nlu/intent_parser.h"
#include "plugins/plugin_manager.h"
#include "utils/config_manager.h"
#include "utils/logger.h"
#include <iostream>

namespace jarvis {

JarvisCore::JarvisCore() = default;

JarvisCore::~JarvisCore() {
    stop();
}

bool JarvisCore::initialize() {
    LOG_INFO("Initializing Jarvis core...");
    
    try {
        // Initialize components
        configManager_ = std::make_unique<ConfigManager>();
        if (!configManager_->load("configs/jarvis.json")) {
            LOG_WARNING("Failed to load configuration, using defaults");
        }

        wakeWordDetector_ = std::make_unique<WakeWordDetector>();
        speechRecognizer_ = std::make_unique<SpeechRecognizer>();
        textToSpeech_ = std::make_unique<TextToSpeech>();
        intentParser_ = std::make_unique<IntentParser>();
        pluginManager_ = std::make_unique<PluginManager>();

        // Initialize wake word detector
        std::string modelPath = configManager_->getString("wake_word.model_path", "models/porcupine_params.pv");
        std::string keywordPath = configManager_->getString("wake_word.keyword_path", "models/hey-jarvis.ppn");
        float sensitivity = configManager_->getFloat("wake_word.sensitivity", 0.5f);

        if (!wakeWordDetector_->initialize(modelPath, keywordPath, sensitivity)) {
            LOG_ERROR("Failed to initialize wake word detector");
            return false;
        }

        // Initialize speech recognizer
        std::string voskModelPath = configManager_->getString("speech_recognition.model_path", "models/vosk-model-en-us-0.22");
        float sampleRate = static_cast<float>(configManager_->getInt("speech_recognition.sample_rate", 16000));
        
        if (!speechRecognizer_->initialize(voskModelPath, sampleRate)) {
            LOG_ERROR("Failed to initialize speech recognizer");
            return false;
        }

        // Initialize text-to-speech
        std::string voice = configManager_->getString("text_to_speech.voice", "en");
        int rate = configManager_->getInt("text_to_speech.rate", 175);
        int volume = configManager_->getInt("text_to_speech.volume", 100);

        if (!textToSpeech_->initialize(voice, rate, volume)) {
            LOG_ERROR("Failed to initialize text-to-speech");
            return false;
        }

        // Initialize plugin manager
        std::string pluginsDir = configManager_->getString("plugins.directory", "plugins");
        bool autoLoad = configManager_->getBool("plugins.auto_load", true);
        
        if (!pluginManager_->initialize(pluginsDir, autoLoad)) {
            LOG_WARNING("Failed to initialize plugin manager");
        }

        // Initialize intent parser
        if (!intentParser_->initialize()) {
            LOG_WARNING("Failed to initialize intent parser");
        }

        LOG_INFO("Jarvis core initialized successfully");
        return true;

    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Exception during initialization: ") + e.what());
        return false;
    }
}

void JarvisCore::start() {
    if (!running_) {
        running_ = true;
        processingThread_ = std::thread(&JarvisCore::processingLoop, this);
        
        // Start wake word detection
        wakeWordDetector_->startDetection([this]() {
            handleWakeWordDetected();
        });
        
        LOG_INFO("Jarvis started");
        textToSpeech_->speak("Jarvis is ready");
    }
}

void JarvisCore::stop() {
    if (running_) {
        running_ = false;
        
        // Stop wake word detection
        if (wakeWordDetector_) {
            wakeWordDetector_->stopDetection();
        }
        
        // Stop speech recognition
        if (speechRecognizer_) {
            speechRecognizer_->stopRecognition();
        }
        
        // Stop processing thread
        if (processingThread_.joinable()) {
            processingThread_.join();
        }
        
        LOG_INFO("Jarvis stopped");
    }
}

void JarvisCore::processCommand(const std::string& command) {
    LOG_INFO("Processing command: " + command);
    
    try {
        // Parse intent
        auto intent = intentParser_->parse(command);
        if (!intent) {
            textToSpeech_->speak("I didn't understand that command");
            return;
        }
        
        // Execute intent through plugin manager
        auto result = pluginManager_->executeIntent(*intent);
        
        if (result.success) {
            if (!result.response.empty()) {
                textToSpeech_->speak(result.response);
            }
        } else {
            textToSpeech_->speak("Sorry, I couldn't complete that task: " + result.error);
        }
        
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Error processing command: ") + e.what());
        textToSpeech_->speak("Sorry, an error occurred while processing your command");
    }
}

void JarvisCore::processingLoop() {
    LOG_INFO("Processing loop started");
    
    while (running_) {
        // Main processing happens here
        // Currently handled by callbacks from wake word detector
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    LOG_INFO("Processing loop stopped");
}

void JarvisCore::handleWakeWordDetected() {
    LOG_INFO("Wake word detected");
    textToSpeech_->speak("Yes?");
    
    // Start speech recognition
    if (speechRecognizer_->startRecognition()) {
        LOG_INFO("Listening for command...");
        
        // This would typically involve audio capture and recognition
        // For now, we'll simulate with a simple example
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        // In a real implementation, this would come from speech recognition
        std::string command = "what time is it";
        
        if (!command.empty()) {
            processCommand(command);
        }
        
        speechRecognizer_->stopRecognition();
    }
}

} // namespace jarvis