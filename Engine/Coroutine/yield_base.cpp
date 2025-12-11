#include "pch.h"
#include "yield_base.h"
#include "task.h"


bool engine::WaitForTask::should_resume()
{
    auto h = task.handle;
    if (auto yield = h.promise().current_yield.get())
    {
        if (!yield->should_resume())
        {
            return false;
        }
    }

    h.resume();

    return h.done();
}

bool engine::WaitForTask::await_ready() const noexcept
{
    return false;
}

void engine::WaitForTask::await_suspend(std::coroutine_handle<> awaiting)
{
    task.handle.promise().waiters.push_back(awaiting);
}