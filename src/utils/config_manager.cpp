#include "utils/config_manager.h"
#include <fstream>
#include <iostream>

namespace jarvis {

ConfigManager::ConfigManager() : loaded_(false) {}

ConfigManager::~ConfigManager() = default;

bool ConfigManager::load(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file >> config_;
        loaded_ = true;
        filename_ = filename;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config file: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::save(const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << config_.dump(4); // Pretty print with 4 spaces
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config file: " << e.what() << std::endl;
        return false;
    }
}

std::string ConfigManager::getString(const std::string& key, const std::string& defaultValue) {
    try {
        if (!loaded_) return defaultValue;
        
        auto keys = splitKey(key);
        nlohmann::json* current = &config_;
        
        for (const auto& k : keys) {
            if (!current->contains(k)) {
                return defaultValue;
            }
            current = &(*current)[k];
        }
        
        return current->get<std::string>();
    } catch (const std::exception&) {
        return defaultValue;
    }
}

int ConfigManager::getInt(const std::string& key, int defaultValue) {
    try {
        if (!loaded_) return defaultValue;
        
        auto keys = splitKey(key);
        nlohmann::json* current = &config_;
        
        for (const auto& k : keys) {
            if (!current->contains(k)) {
                return defaultValue;
            }
            current = &(*current)[k];
        }
        
        return current->get<int>();
    } catch (const std::exception&) {
        return defaultValue;
    }
}

float ConfigManager::getFloat(const std::string& key, float defaultValue) {
    try {
        if (!loaded_) return defaultValue;
        
        auto keys = splitKey(key);
        nlohmann::json* current = &config_;
        
        for (const auto& k : keys) {
            if (!current->contains(k)) {
                return defaultValue;
            }
            current = &(*current)[k];
        }
        
        return current->get<float>();
    } catch (const std::exception&) {
        return defaultValue;
    }
}

bool ConfigManager::getBool(const std::string& key, bool defaultValue) {
    try {
        if (!loaded_) return defaultValue;
        
        auto keys = splitKey(key);
        nlohmann::json* current = &config_;
        
        for (const auto& k : keys) {
            if (!current->contains(k)) {
                return defaultValue;
            }
            current = &(*current)[k];
        }
        
        return current->get<bool>();
    } catch (const std::exception&) {
        return defaultValue;
    }
}

void ConfigManager::setString(const std::string& key, const std::string& value) {
    setValue(key, value);
}

void ConfigManager::setInt(const std::string& key, int value) {
    setValue(key, value);
}

void ConfigManager::setFloat(const std::string& key, float value) {
    setValue(key, value);
}

void ConfigManager::setBool(const std::string& key, bool value) {
    setValue(key, value);
}

std::vector<std::string> ConfigManager::splitKey(const std::string& key) {
    std::vector<std::string> keys;
    std::string current;
    
    for (char c : key) {
        if (c == '.') {
            if (!current.empty()) {
                keys.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        keys.push_back(current);
    }
    
    return keys;
}

void ConfigManager::setValue(const std::string& key, const nlohmann::json& value) {
    try {
        auto keys = splitKey(key);
        nlohmann::json* current = &config_;
        
        for (size_t i = 0; i < keys.size() - 1; ++i) {
            if (!current->contains(keys[i])) {
                (*current)[keys[i]] = nlohmann::json::object();
            }
            current = &(*current)[keys[i]];
        }
        
        (*current)[keys.back()] = value;
    } catch (const std::exception& e) {
        std::cerr << "Error setting config value: " << e.what() << std::endl;
    }
}

} // namespace jarvis