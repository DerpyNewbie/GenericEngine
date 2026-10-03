#pragma once
#include <any>

#include "cancellation_token.h"
#include "task.h"
#include "yield_base.h"

namespace engine
{
//先人がゲームとエンジンのcoroutineは別けたほうがいいよって言ってたのでシングルトンじゃないです
class CoroutineManager
{
    using CoroutineEntry = std::pair<std::coroutine_handle<Task::promise_type>, CancellationToken>;
    std::vector<CoroutineEntry> m_coroutines_;

public:
    void Start(Task&& t)
    {
        t.handle.resume();
        m_coroutines_.emplace_back(t.handle, CancellationToken());
        t.handle = nullptr;
    }
    void Start(Task&& t, CancellationToken token)
    {
        t.handle.resume();
        m_coroutines_.emplace_back(t.handle, token);
        t.handle = nullptr;
    }

    void Update(float dt)
    {
        for (auto it = m_coroutines_.begin(); it != m_coroutines_.end();)
        {
            try
            {
                if (it->second.IsCancellationRequested())
                {
                    it->first.destroy();
                    it = m_coroutines_.erase(it);
                    continue;
                }
                
                auto h = it->first;
                auto& promise = h.promise();

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
            catch(const std::any& e)
            {
                std::cout << e.type().name() << std::endl;
            }
            catch(...)
            {
                std::cout << "unknown exception" << std::endl;
            }
        }
    }
};
}
