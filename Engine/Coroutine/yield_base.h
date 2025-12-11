#pragma once
#include "engine_time.h"

namespace engine
{
struct Task;
class Transform;
struct YieldBase
{
    virtual bool should_resume() = 0;
    virtual ~YieldBase() = default;
};

struct WaitForNextFrame : YieldBase
{
    bool await_ready() const noexcept
    {
        return false;
    }
    bool should_resume() override
    {
        return true;
    }
    void await_suspend(std::coroutine_handle<>) const noexcept
    {}
    void await_resume() const noexcept
    {}
};

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

struct WaitForFrame : YieldBase
{
    uint32_t end_frame_count;

    explicit WaitForFrame(uint32_t frames)
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

struct WaitForTask : YieldBase
{
    Task &task;

    WaitForTask(Task &&t) : task(t)
    {}

    bool should_resume() override;

    bool await_ready() const noexcept;

    void await_suspend(std::coroutine_handle<> awaiting);

    void await_resume() noexcept
    {}
};
}