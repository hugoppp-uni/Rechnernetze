#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>

class Timer {
public:
    void start();
    void start(int timeout);
    void stop();
    bool isRunning();
    double elapsedMilliseconds();
    double elapsedSeconds();
    bool elapsed();

private:
    std::chrono::time_point<std::chrono::system_clock> m_StartTime;
    std::chrono::time_point<std::chrono::system_clock> m_EndTime;
    bool m_Running = false;
    int m_Timeout = 0;
};

