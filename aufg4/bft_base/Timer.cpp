#include "timer.hpp"
#include "logger.hpp"

void Timer::start() {
    m_StartTime = std::chrono::system_clock::now();
    m_Running = true;
    Logger::debug("Timer started");
}

void Timer::stop() {
    m_EndTime = std::chrono::system_clock::now();
    m_Running = false;
    Logger::debug("Timer stopped");
}

double Timer::elapsedMilliseconds() {
    std::chrono::time_point<std::chrono::system_clock> nowTime;

    if(m_Running)
    {
        nowTime = std::chrono::system_clock::now();
    }
    else
    {
        nowTime = m_EndTime;
    }

    double elapsed =(double) std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - m_StartTime).count();
    Logger::debug("Timer elapsed milliseconds: " + std::to_string(elapsed));
    return elapsed;
}

double Timer::elapsedSeconds() {
    double elapsed = elapsedMilliseconds() / 1000.0;
    Logger::debug("Timer elapsed seconds: " + std::to_string(elapsed));
    return elapsed;
}

bool Timer::isRunning() {
    return m_Running;
}

void Timer::start(int timeout) {
    m_Timeout = timeout;
    Logger::debug("Timer started");
}

bool Timer::elapsed() {
    if (elapsedMilliseconds() > m_Timeout) {
        Logger::debug("Timer has elapsed");
        stop();
        return true;
    }
    Logger::debug("Timer has not elapsed");
    return false;
}
