#pragma once
#include "Components/transform.h"
#include "Math/trs.h"

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

struct Tween : YieldBase
{
    std::weak_ptr<Transform> transform;
    TRS from;
    TRS to;
    float time;
    float duration;

    explicit Tween(const std::weak_ptr<Transform> &transform, const TRS &from, const TRS &to, const float duration) : transform(transform), from(from), to(to), time(0), duration(duration)
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