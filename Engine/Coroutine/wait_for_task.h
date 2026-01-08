#pragma once
#include "yield_base.h"

namespace engine
{
struct Task;
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