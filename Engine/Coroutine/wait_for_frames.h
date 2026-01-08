#pragma once
#include "engine_time.h"
#include "yield_base.h"

namespace engine
{
struct WaitForFrames : YieldBase
{
    uint32_t end_frame_count;

    explicit WaitForFrames(uint32_t frames)
    {
        end_frame_count = frames + Time::Get()->Frames();
    }

    bool should_resume() override
    {
        return end_frame_count <= Time::Get()->Frames();
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