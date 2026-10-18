#pragma once
#include "engine_time.h"
#include "yield_base.h"

namespace engine
{
struct WaitForSeconds : YieldBase
{
    bool use_unscaled;
    float end_time;

    static float GetTimeSinceStartUp(bool use_unscaled_time)
    {
        const auto time = use_unscaled_time ? Time::Get()->UnscaledTimeSinceStartUp() : Time::Get()->TimeSinceStartUp();
        return time;
    }
    
    explicit WaitForSeconds(const float t, const bool use_unscaled_time = false) : use_unscaled(use_unscaled_time)
    {
        end_time = t + GetTimeSinceStartUp(use_unscaled);
    }

    bool should_resume() override
    {
        return end_time <= GetTimeSinceStartUp(use_unscaled);
    }
    bool await_ready() const noexcept
    {
        return false;
    }
    void await_suspend(std::coroutine_handle<>) const noexcept
    {}
    void await_resume() const noexcept
    {}

};
}