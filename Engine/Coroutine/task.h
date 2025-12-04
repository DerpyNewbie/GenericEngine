#pragma once
#include <utility>

#include "yield_base.h"

namespace engine
{
struct Task
{
    struct promise_type
    {
        std::unique_ptr<YieldBase> current_yield = nullptr;

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
            Logger::Error<Task>("Coroutine exception: {}", std::current_exception());
        }

        auto await_transform(WaitForNextFrame w)
        {
            current_yield = std::make_unique<WaitForNextFrame>(std::move(w));
            return *static_cast<WaitForNextFrame *>(current_yield.get());
        }

        auto await_transform(WaitForSeconds w)
        {
            current_yield = std::make_unique<WaitForSeconds>(std::move(w));
            return *static_cast<WaitForSeconds *>(current_yield.get());
        }
        auto await_transform(WaitForFrame w)
        {
            current_yield = std::make_unique<WaitForFrame>(std::move(w));
            return *static_cast<WaitForFrame *>(current_yield.get());
        }

        auto await_transform(Tween w)
        {
            current_yield = std::make_unique<Tween>(std::move(w));
            return *static_cast<Tween *>(current_yield.get());
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