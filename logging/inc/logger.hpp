#ifndef _logger_hpp_
#define _logger_hpp_

#include <mutex>
#include <sstream>
#include <iostream>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    template<typename ... Args>
    static void info(Args&&... args) {
        write(LogLevel::INFO, std::forward<Args>(args)...);
    }

    template<typename ... Args>
    static void warning(Args&&... args) {
        write(LogLevel::WARNING, std::forward<Args>(args)...);
    }

    template<typename ... Args>
    static void error(Args&&... args) {
        write(LogLevel::ERROR, std::forward<Args>(args)...);
    }

private:
    template<typename ... Args>
    static void write(LogLevel level, Args&&... args) {
        std::ostringstream oss;

        switch (level) {
        case LogLevel::INFO:
            oss << "[INFO] ";
            break;
        case LogLevel::WARNING:
            oss << "[WARNING] ";
            break;
        case LogLevel::ERROR:
            oss << "[ERROR] ";
            break;
        default:
            oss << "[UNKNOWN] ";
            break;
        }

        (oss << ... << std::forward<Args>(args));

        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << oss.str() << '\n';
    }

    static std::mutex mutex_;
};

#endif