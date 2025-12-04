#pragma once
#include "Components/transform.h"
#include "task.h"
#include "yield_base.h"
#include "Math/trs.h"

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

            if (auto *w = dynamic_cast<Tween *>(promise.current_yield.get()))
            {
                w->time += dt;
                //もしもdurationが0だった場合ここで抜けるから大丈夫！
                if (w->time < w->duration)
                {
                    auto t = std::clamp(w->time / w->duration, 0.0f, 1.0f);
                    auto blended = TRS::Blend(w->from, w->to, t);

                    w->transform.lock()->SetLocalMatrix(blended.GetMatrix());
                    ++it;
                    continue;
                }
                w->transform.lock()->SetLocalMatrix(w->to.GetMatrix());

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