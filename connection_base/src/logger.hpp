#pragma once

#include <iostream>
#include <iomanip>


class Logger {

public:
    enum class level {
        ERR = 3,
        WARN = 2,
        INFO = 1,
        DATA = 0,
    };

private:

    std::string logfile{};
    bool log_to_console{};
    level minimal_log_level{level::INFO};

    static Logger &instance() {
        static Logger instance;
        return instance;
    }

    static std::string level_str(level l) {
        switch (l) {
            case level::ERR:
                return "[ERR] ";
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
        if (log_level < this->minimal_log_level)
            return;

        std::stringstream stringstream;

        auto t = std::time(nullptr);
        auto local_time = *std::localtime(&t);

        stringstream << level_str(log_level);
        stringstream << '[' << std::put_time(&local_time, "%F %T") << "] ";
        stringstream << log << std::endl;

        if (log_to_console)
            std::cout << stringstream.str() << std::endl;
        if (!logfile.empty());//todo
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

    static void set_logfile(const std::string &logfile) {
        instance().logfile = logfile;
    }

    static void set_log_to_console(bool log_to_console) {
        instance().log_to_console = log_to_console;
    }

    static void set_level(level log_level) {
        instance().minimal_log_level = log_level;
    }

};