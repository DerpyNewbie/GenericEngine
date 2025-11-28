#pragma once

namespace engine
{
struct YieldBase
{
    virtual ~YieldBase() = default;
};

struct WaitForNextFrame : YieldBase
{
    bool await_ready() const noexcept
    {
        return false;
    }
    void await_suspend(std::coroutine_handle<>) const noexcept
    {}
    void await_resume() const noexcept
    {}
};

struct WaitForSeconds : YieldBase
{
    float time;
    explicit WaitForSeconds(float t) : time(t)
    {}
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
    uint32_t remaining_frames;

    explicit WaitForFrame(uint32_t frames) : remaining_frames(frames)
    {}
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