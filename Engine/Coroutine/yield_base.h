#pragma once

namespace engine
{
/// <summary>
/// Coroutineを待機させる条件の基底構造体です。
/// </summary>
struct YieldBase
{
    /// <summary>
    /// 待機条件を満たし、Coroutineを再開してよいかどうかを返します。
    /// </summary>
    virtual bool should_resume() = 0;
    virtual ~YieldBase() = default;
};
}