#ifndef SPN_LOGGER_H
#define SPN_LOGGER_H
//AI generated code
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <ctime>
#include <iomanip>

namespace spn {
    enum class LogLevel { INFO, WARNING, ERROR, DEBUG };

    class Logger {
    public:
        // Access the single instance of the logger
        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }

        // Set the output file
        void init(const std::string& filename) {
            std::unique_lock<std::mutex> lock(logMutex);
            if (logFile.is_open()) logFile.close();
            logFile.open(filename, std::ios::app);
        }

        // Thread-safe logging function
        template<typename... Args>
        void log(LogLevel level, Args... args) {
            std::unique_lock<std::mutex> lock(logMutex);

            std::ostream& out = logFile.is_open() ? logFile : std::cout;

            out << "[" << getTimestamp() << "] "
                << "[" << levelToString(level) << "] ";

            // Unpack variadic arguments using a fold-expression style for C++11
            int dummy[] = { 0, (void(out << args << " "), 0)... };
            static_cast<void>(dummy);

            out << std::endl;
        }

    private:
        Logger() = default;
        ~Logger() {
            if (logFile.is_open()) logFile.close();
        }

        // Prevent copying
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        std::ofstream logFile;
        std::mutex logMutex;

        std::string levelToString(LogLevel level) {
            switch (level) {
            case LogLevel::DEBUG:   return "DEBUG";
            case LogLevel::INFO:    return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR:   return "ERROR";
            default:                return "UNKNOWN";
            }
        }

        std::string getTimestamp() {
            std::time_t now = std::time(nullptr);
            std::tm tm = *std::localtime(&now);
            char buf[20];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
            return std::string(buf);
        }
    };

    // Convenience Macros
#define LOG_INFO(...)    spn::Logger::getInstance().log(spn::LogLevel::INFO, __VA_ARGS__)
#define LOG_WARN(...)    spn::Logger::getInstance().log(spn::LogLevel::WARNING, __VA_ARGS__)
#define LOG_ERROR(...)   spn::Logger::getInstance().log(spn::LogLevel::ERROR, __VA_ARGS__)
#define LOG_DEBUG(...)   spn::Logger::getInstance().log(spn::LogLevel::DEBUG, __VA_ARGS__)

}

//usage
/*
#include <thread>
#include <vector>
#include "spn_logger.h"

void task(int id) {
    LOG_INFO("Task", id, "is starting...");
}

int main() {
    // Optional: Initialize to a file, otherwise defaults to std::cout
    spn::Logger::getInstance().init("app_log.txt");

    LOG_INFO("Application started.");
    LOG_WARN("This is a warning with a value:", 42);

    // Testing thread safety
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(task, i);
    }

    for (auto& t : threads) t.join();

    LOG_ERROR("Application shutting down.");
    return 0;
}
*/

#endif