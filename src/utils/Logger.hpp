#pragma once

#include <chrono>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

namespace pyro {
    enum class LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

    inline std::string logLevelToString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG:
                return "DEBUG";
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARNING:
                return "WARNING";
            case LogLevel::ERROR:
                return "ERROR";
            case LogLevel::CRITICAL:
                return "CRITICAL";
            default:
                return "UNKNOWN";
        }
    }

    class Logger {
    public:
        static Logger &getInstance() {
            static Logger instance;
            return instance;
        }

#ifdef PYRO_DEBUG // PYRO_DEBUG is defined
        void setLogLevel(LogLevel level) { minLogLevel = level; }

        void enableFileLogging(const std::string &filename) {
            std::lock_guard<std::mutex> lock(mutex_);
            logFile.open(filename, std::ios::out | std::ios::app);
            if (!logFile) {
                std::cerr << "[Logger] Failed to open log file: " << filename << std::endl;
            }
        }

        void log(LogLevel level, const std::string &message, const char *file, int line) {
            if (level < minLogLevel)
                return;

            std::ostringstream logStream;
            logStream << getCurrentTime() << " [" << logLevelToString(level) << "] " << file << ":" << line << " - "
                      << message << "\n";

            std::string logMessage = logStream.str();
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (level >= LogLevel::WARNING)
                    std::cerr << logMessage << "\n";
                else
                    std::cout << logMessage;
                if (logFile.is_open()) {
                    logFile << logMessage;
                }
            }
        }

        void assertFailure(const char *expr, const char *file, int line) {
            std::cerr << "[ASSERTION FAILED] " << expr << " at " << file << ":" << line << std::endl;
            if (logFile.is_open())
                logFile << "[ASSERTION FAILED] " << expr << " at " << file << ":" << line << std::endl;
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
#endif // DEBUG
    };
} // namespace pyro

// Logging Macros

#ifdef ENABLE_LOGGING
#define LOG(level, msg, ...) pyro::Logger::getInstance().log(level, std::format(msg, ##__VA_ARGS__), __FILE__, __LINE__)
#else
#define LOG(level, msg, ...) ((void) 0)
#endif


#ifdef PYRO_DEBUG
#define ASSERT_EQUAL(expr, cmp, msg, ...)                                                                                    \
    LOG(pyro::LogLevel::DEBUG, "Assert: {}=={}", #expr, #cmp);                                                         \
    if ((expr) != (cmp)) {                                                                                             \
        LOG(pyro::LogLevel::DEBUG, msg, ##__VA_ARGS__);                                                                \
        pyro::Logger::getInstance().assertFailure(#expr, __FILE__, __LINE__);                                          \
    }
#else
#define ASSERT_EQUAL(expr, cmp, msg, ...) ((expr));
#endif
