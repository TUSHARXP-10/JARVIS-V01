#include <iostream>
#include <csignal>
#include <memory>
#include "core/jarvis_core.h"
#include "utils/logger.h"
#include "utils/config_manager.h"

using namespace jarvis;

std::atomic<bool> running(true);

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
        running = false;
    }
}

int main(int argc, char* argv[]) {
    // Set up signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::cout << "==========================================" << std::endl;
    std::cout << "        Jarvis Voice Assistant v1.0.0     " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Initializing..." << std::endl;

    // Initialize logger
    auto& logger = Logger::getInstance();
    if (!logger.initialize("jarvis.log", LogLevel::INFO)) {
        std::cerr << "Failed to initialize logger" << std::endl;
        return 1;
    }

    LOG_INFO("Jarvis starting up...");

    // Load configuration
    ConfigManager config;
    if (!config.load("configs/jarvis.json")) {
        LOG_WARNING("Failed to load configuration file, using defaults");
    }

    try {
        // Create and initialize Jarvis core
        auto jarvis = std::make_unique<JarvisCore>();
        
        if (!jarvis->initialize()) {
            LOG_ERROR("Failed to initialize Jarvis core");
            return 1;
        }

        LOG_INFO("Jarvis initialized successfully");
        std::cout << "Say 'Hey Jarvis' to activate voice commands!" << std::endl;
        std::cout << "Press Ctrl+C to exit" << std::endl;

        // Start the voice assistant
        jarvis->start();

        // Main loop
        while (running && jarvis->isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Stop the voice assistant
        jarvis->stop();
        LOG_INFO("Jarvis shutting down...");

    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Exception: ") + e.what());
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    LOG_INFO("Jarvis shutdown complete");
    return 0;
}