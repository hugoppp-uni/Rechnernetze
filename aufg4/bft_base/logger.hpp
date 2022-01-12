#pragma once

#include <iostream>
#include <iomanip>
#include <mutex>


class Logger {

public:
    enum class level {
        DEBUG = 4,
        ERR = 3,
        WARN = 2,
        INFO = 1,
        DATA = 0,
    };

private:

    std::mutex mutex{};
    level minimal_log_level{level::INFO};

    static Logger &instance() {
        static Logger instance;
        return instance;
    }

    static std::string level_str(level l) {
        switch (l) {
            case level::DEBUG:
                return "[DEBUG] ";
            case level::ERR:
                return "[ERROR] ";
            case level::WARN:
                return "[WARN] ";
            case level::INFO:
                return "[INFO] ";
            case level::DATA:
                return "[DATA] ";
            default:
                return "[UNKNOWN] ";
        }
    }

    void log_internal(const std::string &log, level log_level) {
        std::lock_guard<std::mutex> lock{mutex};
        if (log_level < this->minimal_log_level)
            return;

        std::stringstream stringstream;

        stringstream << level_str(log_level);
        {
            auto t = std::time(nullptr);
            //localtime is not thread safe
            auto local_time = *std::localtime(&t);
            stringstream << '[' << std::put_time(&local_time, "%F %T") << "] ";
        }
        stringstream << log;

        if(log_level == level::DEBUG || log_level == level::ERR) {
            std::cerr << stringstream.str() << std::endl;
        } else {
            std::cout << stringstream.str() << std::endl;
        }
    }

public:
    static void info(const std::string &log) {
        Logger::instance().log_internal(log, level::INFO);
    }

    static void error(const std::string &log) {
        Logger::instance().log_internal(log, level::ERR);
    }

    static void warn(const std::string &log) {
        Logger::instance().log_internal(log, level::WARN);
    }

    static void data(const std::string &log) {
        Logger::instance().log_internal(log, level::DATA);
    }

    static void debug(const std::string &log) {
        Logger::instance().log_internal(log, level::DEBUG);
    }

    static void set_level(level log_level) {
        instance().minimal_log_level = log_level;
    }

};