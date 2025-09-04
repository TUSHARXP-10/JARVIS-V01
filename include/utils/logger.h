#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <mutex>

namespace jarvis {

/**
 * @brief Logging levels
 */
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

/**
 * @brief Logger class for thread-safe logging
 */
class Logger {
public:
    static Logger& getInstance();

    /**
     * @brief Initialize the logger
     * @param filename Log file name
     * @param level Minimum log level
     * @return true if initialization successful, false otherwise
     */
    bool initialize(const std::string& filename, LogLevel level = LogLevel::INFO);

    /**
     * @brief Log a debug message
     * @param message Message to log
     * @param file Source file name
     * @param line Line number
     */
    void debug(const std::string& message, const char* file = nullptr, int line = 0);

    /**
     * @brief Log an info message
     * @param message Message to log
     * @param file Source file name
     * @param line Line number
     */
    void info(const std::string& message, const char* file = nullptr, int line = 0);

    /**
     * @brief Log a warning message
     * @param message Message to log
     * @param file Source file name
     * @param line Line number
     */
    void warning(const std::string& message, const char* file = nullptr, int line = 0);

    /**
     * @brief Log an error message
     * @param message Message to log
     * @param file Source file name
     * @param line Line number
     */
    void error(const std::string& message, const char* file = nullptr, int line = 0);

    /**
     * @brief Log a critical message
     * @param message Message to log
     * @param file Source file name
     * @param line Line number
     */
    void critical(const std::string& message, const char* file = nullptr, int line = 0);

    /**
     * @brief Set minimum log level
     * @param level Minimum log level
     */
    void setLevel(LogLevel level);

    /**
     * @brief Check if logger is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const { return initialized_; }

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(LogLevel level, const std::string& message, const char* file, int line);
    std::string levelToString(LogLevel level);
    std::string getCurrentTimestamp();

    bool initialized_;
    std::unique_ptr<std::ofstream> logFile_;
    LogLevel minLevel_;
    std::mutex logMutex_;
};

// Convenience macros for logging
#define LOG_DEBUG(msg) jarvis::Logger::getInstance().debug(msg, __FILE__, __LINE__)
#define LOG_INFO(msg) jarvis::Logger::getInstance().info(msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) jarvis::Logger::getInstance().warning(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) jarvis::Logger::getInstance().error(msg, __FILE__, __LINE__)
#define LOG_CRITICAL(msg) jarvis::Logger::getInstance().critical(msg, __FILE__, __LINE__)

} // namespace jarvis