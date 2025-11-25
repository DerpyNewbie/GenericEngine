#pragma once
#include "yield_base.h"

namespace engine
{
struct Task
{
    struct promise_type
    {
        YieldBase *current_yield = nullptr;

        Task get_return_object()
        {
            return Task(std::coroutine_handle<promise_type>::from_promise(*this));
        }

        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }
        std::suspend_always final_suspend() noexcept
        {
            return {};
        }
        void return_void()
        {}
        void unhandled_exception()
        {
            std::terminate();
        }

        auto await_transform(WaitForNextFrame w)
        {
            delete current_yield;
            current_yield = new WaitForNextFrame(w);
            return *static_cast<WaitForNextFrame *>(current_yield);
        }

        auto await_transform(WaitForSeconds w)
        {
            delete current_yield;
            current_yield = new WaitForSeconds(w);
            return *static_cast<WaitForSeconds *>(current_yield);
        }
    };

    std::coroutine_handle<promise_type> handle;

    explicit Task(std::coroutine_handle<promise_type> h) : handle(h)
    {}
    Task(Task &&t) noexcept : handle(t.handle)
    {
        t.handle = {};
    }
    ~Task()
    {
        if (handle)
            handle.destroy();
    }
};
}