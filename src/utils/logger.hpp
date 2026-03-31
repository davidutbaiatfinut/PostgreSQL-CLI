#pragma once
#include <string>
#include <fstream>
#include <mutex>

namespace pgcli::utils {

enum class LogLevel { INFO, WARNING, ERROR };

class Logger {
public:
    static Logger& getInstance();
    
    void setLogFile(const std::string& filePath);
    
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);

private:
    void log(LogLevel level, const std::string& msg);

    Logger() = default;
    ~Logger() {
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream logFile_;
    std::mutex mtx_;
};

} // namespace pgcli::utils
