#pragma once
#include "yield_base.h"

namespace engine
{
/// <summary>
/// 次のフレームまでCoroutineを待機させます。
/// </summary>
struct WaitForNextFrame : YieldBase
{
    /// <summary>
    /// 常に false を返し、Coroutineを必ず中断させます。
    /// </summary>
    bool await_ready() const noexcept
    {
        return false;
    }
    /// <summary>
    /// 常に true を返します。次のCoroutineManagerのUpdateで再開されます。
    /// </summary>
    bool should_resume() override
    {
        return true;
    }
    /// <summary>
    /// 何もしません。再開はCoroutineManagerが行います。
    /// </summary>
    void await_suspend(std::coroutine_handle<>) const noexcept
    {}
    /// <summary>
    /// 何もしません。
    /// </summary>
    void await_resume() const noexcept
    {}
};
}