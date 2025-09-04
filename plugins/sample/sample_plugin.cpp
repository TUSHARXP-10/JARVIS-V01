#include "core/plugin.h"
#include "utils/logger.h"
#include <chrono>
#include <ctime>

namespace jarvis {

class SamplePlugin : public IPlugin {
public:
    bool initialize(const std::string& configPath) override {
        LOG_INFO("Sample plugin initialized");
        return true;
    }
    
    std::string getName() const override {
        return "sample";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string handleIntent(const Intent& intent) override {
        if (intent.name == "time_query") {
            return handleTimeQuery(intent);
        } else if (intent.name == "greeting") {
            return handleGreeting(intent);
        }
        return "I don't know how to handle that yet.";
    }
    
    std::map<std::string, std::function<std::string(const Intent&)>> getIntentHandlers() override {
        return {
            {"time_query", [this](const Intent& intent) { return handleTimeQuery(intent); }},
            {"greeting", [this](const Intent& intent) { return handleGreeting(intent); }}
        };
    }
    
    void shutdown() override {
        LOG_INFO("Sample plugin shutting down");
    }

private:
    std::string handleTimeQuery(const Intent& intent) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%I:%M %p", std::localtime(&time_t));
        
        return "According to my sample plugin, the time is " + std::string(buffer);
    }
    
    std::string handleGreeting(const Intent& intent) {
        return "Hello from the sample plugin! I'm here to demonstrate how plugins work.";
    }
};

} // namespace jarvis

// Plugin factory functions
extern "C" {
    jarvis::IPlugin* createPlugin() {
        return new jarvis::SamplePlugin();
    }
    
    void destroyPlugin(jarvis::IPlugin* plugin) {
        delete plugin;
    }
}