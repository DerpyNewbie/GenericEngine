#pragma once
#include "yield_base.h"

namespace engine
{
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
}