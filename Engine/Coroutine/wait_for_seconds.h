#pragma once
#include "engine_time.h"
#include "yield_base.h"

namespace engine
{
/// <summary>
/// 指定した秒数だけCoroutineを待機させます。
/// </summary>
struct WaitForSeconds : YieldBase
{
    float end_time;
    /// <summary>
    /// 指定された秒数だけ待機する条件を作成します。TimeScaleの影響を受けます。
    /// </summary>
    /// <param name="t">待機する秒数</param>
    explicit WaitForSeconds(const float t)
    {
        end_time = t + static_cast<float>(Time::Get()->TimeSinceStartUp());
    }

    /// <summary>
    /// 指定された秒数が経過したかどうかを返します。
    /// </summary>
    bool should_resume() override
    {
        return end_time <= static_cast<float>(Time::Get()->TimeSinceStartUp());
    }
    /// <summary>
    /// 常に false を返し、Coroutineを必ず中断させます。
    /// </summary>
    bool await_ready() const noexcept
    {
        return false;
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