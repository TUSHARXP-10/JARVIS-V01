#pragma once

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace jarvis {

/**
 * @brief Configuration manager for Jarvis
 * 
 * Manages configuration settings from JSON files
 */
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();

    /**
     * @brief Load configuration from file
     * @param filename Configuration file path
     * @return true if loaded successfully, false otherwise
     */
    bool load(const std::string& filename);

    /**
     * @brief Save configuration to file
     * @param filename Configuration file path
     * @return true if saved successfully, false otherwise
     */
    bool save(const std::string& filename);

    /**
     * @brief Get string configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     */
    std::string getString(const std::string& key, const std::string& defaultValue = "");

    /**
     * @brief Get integer configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     */
    int getInt(const std::string& key, int defaultValue = 0);

    /**
     * @brief Get float configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     */
    float getFloat(const std::string& key, float defaultValue = 0.0f);

    /**
     * @brief Get boolean configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     */
    bool getBool(const std::string& key, bool defaultValue = false);

    /**
     * @brief Set string configuration value
     * @param key Configuration key
     * @param value Value to set
     */
    void setString(const std::string& key, const std::string& value);

    /**
     * @brief Set integer configuration value
     * @param key Configuration key
     * @param value Value to set
     */
    void setInt(const std::string& key, int value);

    /**
     * @brief Set float configuration value
     * @param key Configuration key
     * @param value Value to set
     */
    void setFloat(const std::string& key, float value);

    /**
     * @brief Set boolean configuration value
     * @param key Configuration key
     * @param value Value to set
     */
    void setBool(const std::string& key, bool value);

    /**
     * @brief Check if configuration is loaded
     * @return true if loaded, false otherwise
     */
    bool isLoaded() const { return loaded_; }

    /**
     * @brief Get the underlying JSON object
     * @return Reference to the JSON object
     */
    nlohmann::json& getConfig() { return config_; }

    /**
     * @brief Get the underlying JSON object (const)
     * @return Const reference to the JSON object
     */
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool loaded_;
    std::string filename_;
};

} // namespace jarvis