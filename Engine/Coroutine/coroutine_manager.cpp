#include "pch.h"
#include "coroutine_manager.h"

namespace engine
{
void CoroutineManager::Start(Task &&t)
{
    Start(std::move(t), CancellationToken());
}

void CoroutineManager::Start(Task &&t, CancellationToken token)
{
    t.handle.resume();
    m_coroutines_.emplace_back(t.handle, token);
    t.handle = nullptr;
}

void CoroutineManager::Update()
{
    for (auto it = m_coroutines_.begin(); it != m_coroutines_.end();)
    {
        //キャンセルされていたら消す
        if (it->second.IsCancellationRequested())
        {
            it->first.destroy();
            it = m_coroutines_.erase(it);
            continue;
        }

        auto h = it->first;
        auto &promise = h.promise();

        if (auto yield = promise.current_yield.get())
        {
            //再開可能か確認
            if (!yield->should_resume())
            {
                ++it;
                continue;
            }
        }

        h.resume();

        //終了したら消す
        if (h.done())
        {
            h.destroy();
            it = m_coroutines_.erase(it);
            continue;
        }
        ++it;
    }
}
}