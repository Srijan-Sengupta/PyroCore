#pragma once

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <format>

namespace pyro {
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

    inline std::string logLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }

    class Logger {
    public:
        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }

#ifdef PYRO_DEBUG  // PYRO_DEBUG is defined
        void setLogLevel(LogLevel level) {
            minLogLevel = level;
        }

        void enableFileLogging(const std::string& filename) {
            std::lock_guard<std::mutex> lock(mutex_);
            logFile.open(filename, std::ios::out | std::ios::app);
            if (!logFile) {
                std::cerr << "[Logger] Failed to open log file: " << filename << std::endl;
            }
        }

        void log(LogLevel level, const std::string& message, const char* file, int line) {
            if (level < minLogLevel) return;

            std::ostringstream logStream;
            logStream << getCurrentTime() << " [" << logLevelToString(level) << "] "
                      << file << ":" << line << " - " << message << "\n";

            std::string logMessage = logStream.str();

            {
                std::lock_guard<std::mutex> lock(mutex_);
                std::cout << logMessage;
                if (logFile.is_open()) {
                    logFile << logMessage;
                }
            }
        }
        void assertFailure(const char* expr, const char* file, int line) {
            std::cerr << "[ASSERTION FAILED] " << expr << " at " << file << ":" << line << std::endl;
            if (logFile.is_open()) logFile << "[ASSERTION FAILED] " << expr << " at " << file << ":" << line << std::endl;
            std::abort();
        }

    private:
        Logger() : minLogLevel(LogLevel::DEBUG) {}

        std::string getCurrentTime() {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
            localtime_r(&time, &tm);
            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
            return oss.str();
        }

        LogLevel minLogLevel;
        std::ofstream logFile;
        std::mutex mutex_;
#endif  // DEBUG
    };
}
// Logging Macros

#ifdef PYRO_DEBUG
#define LOG(level, msg, ...) pyro::Logger::getInstance().log(level, std::format(msg, ##__VA_ARGS__), __FILE__, __LINE__)
#else
#define LOG(level, msg, ...) ((void)0)
#endif


#ifdef PYRO_DEBUG
#define ASSERT(expr) \
LOG(pyro::LogLevel::DEBUG, "Checking if {} is aserted.", #expr); \
if (!(expr)) { pyro::Logger::getInstance().assertFailure(#expr, __FILE__, __LINE__); }
#else
#define ASSERT(expr) ((void)0)
#endif
