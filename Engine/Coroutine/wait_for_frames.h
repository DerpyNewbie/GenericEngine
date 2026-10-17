#pragma once
#include "engine_time.h"
#include "yield_base.h"

namespace engine
{
/// <summary>
/// 指定したフレーム数だけCoroutineを待機させます。
/// </summary>
struct WaitForFrames : YieldBase
{
    uint32_t end_frame_count;

    /// <summary>
    /// 指定されたフレーム数だけ待機する条件を作成します。
    /// </summary>
    /// <param name="frames">待機するフレーム数</param>
    explicit WaitForFrames(uint32_t frames)
    {
        end_frame_count = frames + Time::Get()->Frames();
    }

    /// <summary>
    /// 指定されたフレーム数が経過したかどうかを返します。
    /// </summary>
    bool should_resume() override
    {
        return end_frame_count <= Time::Get()->Frames();
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