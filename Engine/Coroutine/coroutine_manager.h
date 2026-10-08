#pragma once
#include "task.h"
#include "yield_base.h"

namespace engine
{
//先人がゲームとエンジンのcoroutineは別けたほうがいいよって言ってたのでシングルトンじゃないです
class CoroutineManager
{
    std::vector<std::coroutine_handle<Task::promise_type>> m_coroutines_;

public:
    /// <summary>
    /// Coroutineを最初の中断点まで実行し、管理対象に追加します。Coroutineの所有権はCoroutineManagerに移ります。
    /// </summary>
    /// <param name="t">開始するCoroutine</param>
    void Start(Task &&t)
    {
        t.handle.resume();
        m_coroutines_.emplace_back(t.handle);
        t.handle = nullptr;
    }

    /// <summary>
    /// 管理しているすべてのCoroutineについて、待機条件を満たしているものを再開し、完了したものを破棄します。
    /// </summary>
    /// <param name="dt">前フレームからの経過時間(現在は使われていません)</param>
    void Update(float dt)
    {
        for (auto it = m_coroutines_.begin(); it != m_coroutines_.end();)
        {
            auto h = *it;
            auto &promise = h.promise();

            if (auto yield = promise.current_yield.get())
            {
                if (!yield->should_resume())
                {
                    ++it;
                    continue;
                }
            }

            h.resume();

            if (h.done())
            {
                h.destroy();
                it = m_coroutines_.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
};
}