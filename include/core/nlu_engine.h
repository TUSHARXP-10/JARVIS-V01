#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>

namespace jarvis {

struct Intent {
    std::string name;
    std::map<std::string, std::string> slots;
    double confidence = 1.0;
};

class NLUEngine {
public:
    using IntentHandler = std::function<std::string(const Intent&)>;

    NLUEngine();
    ~NLUEngine();

    bool initialize(const std::string& configPath);
    Intent parse(const std::string& text);
    
    void registerIntent(const std::string& intent, IntentHandler handler);
    void unregisterIntent(const std::string& intent);

    // Built-in intents
    std::string handleGreeting(const Intent& intent);
    std::string handleTimeQuery(const Intent& intent);
    std::string handleFileOpen(const Intent& intent);
    std::string handleWebSearch(const Intent& intent);
    std::string handleUnknown(const Intent& intent);

private:
    std::map<std::string, IntentHandler> intentHandlers_;
    
    // Simple rule-based parsing for now
    Intent parseGreeting(const std::string& text);
    Intent parseTimeQuery(const std::string& text);
    Intent parseFileOpen(const std::string& text);
    Intent parseWebSearch(const std::string& text);
    
    std::vector<std::string> tokenize(const std::string& text);
    std::string toLower(const std::string& text);
    bool contains(const std::string& text, const std::string& word);
};

} // namespace jarvis