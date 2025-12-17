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
    void Start(Task &&t)
    {
        t.handle.resume();
        m_coroutines_.emplace_back(t.handle);
        t.handle = nullptr;
    }

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