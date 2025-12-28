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
        m_coroutines_.emplace_back(t.handle);
        t.handle = nullptr;
    }

    void Update(float dt)
    {
        for (auto it = m_coroutines_.begin(); it != m_coroutines_.end();)
        {
            auto h = *it;
            auto &promise = h.promise();

            if (auto *w = dynamic_cast<WaitForSeconds *>(promise.current_yield.get()))
            {
                w->time -= dt;
                if (w->time > 0)
                {
                    ++it;
                    continue;
                }
            }

            if (auto *w = dynamic_cast<WaitForFrame *>(promise.current_yield.get()))
            {
                w->remaining_frames--;
                if (w->remaining_frames > 0)
                {
                    ++it;
                    continue;
                }
            }
            if (promise.current_yield == nullptr)
            {
                h.destroy();
                it = m_coroutines_.erase(it);
                continue;
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