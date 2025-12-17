#pragma once
#include "engine_time.h"
#include "yield_base.h"

namespace engine
{
struct WaitForSeconds : YieldBase
{
    float end_time;
    explicit WaitForSeconds(const float t)
    {
        end_time = t + static_cast<float>(Time::Get()->TimeSinceStartUp());
    }

    bool should_resume() override
    {
        return end_time <= static_cast<float>(Time::Get()->TimeSinceStartUp());
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