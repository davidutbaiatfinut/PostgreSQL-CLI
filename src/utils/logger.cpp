#include "logger.hpp"
#include <iostream>
#include <chrono>

namespace pgcli::utils {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLogFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (logFile_.is_open()) {
        logFile_.close();
    }
    logFile_.open(filePath, std::ios_base::app);
}

void Logger::log(LogLevel level, const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx_);
    
    // Get timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t cnow = std::chrono::system_clock::to_time_t(now);
    
    std::string levelStr;
    bool isError = false;
    switch (level) {
        case LogLevel::INFO:    
            levelStr = "[INFO] "; 
            break;
        case LogLevel::WARNING: 
            levelStr = "[WARN] "; 
            break;
        case LogLevel::ERROR:   
            levelStr = "[ERROR]"; 
            isError = true;
            break;
    }

    std::string timeStr = std::ctime(&cnow);
    if (!timeStr.empty()) {
        timeStr.pop_back(); // Remove default newline
    }

    std::string output = timeStr + " " + levelStr + " " + msg + "\n";
    
    if (isError) {
        std::cerr << output;
    } else {
        std::cout << output;
    }

    if (logFile_.is_open()) {
        logFile_ << output;
        logFile_.flush();
    }
}

void Logger::info(const std::string& msg) {
    log(LogLevel::INFO, msg);
}

void Logger::warn(const std::string& msg) {
    log(LogLevel::WARNING, msg);
}

void Logger::error(const std::string& msg) {
    log(LogLevel::ERROR, msg);
}

} // namespace pgcli::utils
