#include "Timer.h"

Timer* g_Timer;

Timer::Timer()
{
    prevTime = std::chrono::high_resolution_clock::now();
}

float Timer::GetDeltaTime()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = currentTime - prevTime;
    prevTime = currentTime;
    return elapsed.count(); // 秒単位の float
}
