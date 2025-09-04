#include "utils/logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace jarvis {

Logger::Logger() : initialized_(false), minLevel_(LogLevel::INFO) {}

Logger::~Logger() {
    if (logFile_) {
        logFile_->close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

bool Logger::initialize(const std::string& filename, LogLevel level) {
    try {
        logFile_ = std::make_unique<std::ofstream>(filename, std::ios::app);
        if (!logFile_->is_open()) {
            std::cerr << "Failed to open log file: " << filename << std::endl;
            return false;
        }
        
        minLevel_ = level;
        initialized_ = true;
        
        info("Logger initialized", __FILE__, __LINE__);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Logger initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void Logger::log(LogLevel level, const std::string& message, const char* file, int line) {
    if (!initialized_ || level < minLevel_) {
        return;
    }

    std::lock_guard<std::mutex> lock(logMutex_);
    
    std::stringstream ss;
    ss << "[" << getCurrentTimestamp() << "] "
       << levelToString(level) << ": " << message;
    
    if (file && line > 0) {
        ss << " (" << file << ":" << line << ")";
    }
    
    ss << std::endl;
    
    *logFile_ << ss.str();
    logFile_->flush();
    
    // Also output to console for important messages
    if (level >= LogLevel::WARNING) {
        std::cout << ss.str();
    }
}

void Logger::debug(const std::string& message, const char* file, int line) {
    log(LogLevel::DEBUG, message, file, line);
}

void Logger::info(const std::string& message, const char* file, int line) {
    log(LogLevel::INFO, message, file, line);
}

void Logger::warning(const std::string& message, const char* file, int line) {
    log(LogLevel::WARNING, message, file, line);
}

void Logger::error(const std::string& message, const char* file, int line) {
    log(LogLevel::ERROR, message, file, line);
}

void Logger::critical(const std::string& message, const char* file, int line) {
    log(LogLevel::CRITICAL, message, file, line);
}

void Logger::setLevel(LogLevel level) {
    minLevel_ = level;
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARNING";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

} // namespace jarvis