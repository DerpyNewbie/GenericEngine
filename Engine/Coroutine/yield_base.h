#pragma once

namespace engine
{
struct YieldBase
{
    virtual ~YieldBase() = default;
    virtual bool ShouldResume(float dt) = 0 =
    0;
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

    bool ShouldResume(float dt) override
    {
        return true;
    }
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

    bool ShouldResume(float dt) override
    {
        time -= dt;
        if (time > 0)
        {
            return false;
        }
        return true;
    }
};
}