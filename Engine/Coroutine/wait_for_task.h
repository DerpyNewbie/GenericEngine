#pragma once
#include "yield_base.h"

namespace engine
{
struct Task;
/// <summary>
/// 別のTaskが完了するまでCoroutineを待機させます。
/// </summary>
struct WaitForTask : YieldBase
{
    Task &task;

    /// <summary>
    /// 別のTaskの完了を待機する条件を作成します。
    /// </summary>
    /// <param name="t">完了を待つTask</param>
    WaitForTask(Task &&t) : task(t)
    {}

    /// <summary>
    /// 待っているTaskを進め、そのTaskが完了したかどうかを返します。
    /// </summary>
    bool should_resume() override;

    /// <summary>
    /// 常に false を返し、Coroutineを必ず中断させます。
    /// </summary>
    bool await_ready() const noexcept;

    /// <summary>
    /// 待っているTaskのwaitersに、中断したCoroutineを登録します。
    /// </summary>
    /// <param name="awaiting">中断したCoroutine</param>
    void await_suspend(std::coroutine_handle<> awaiting);

    /// <summary>
    /// 何もしません。
    /// </summary>
    void await_resume() noexcept
    {}
};
}