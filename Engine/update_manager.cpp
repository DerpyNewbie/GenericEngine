#include "pch.h"

#include "update_manager.h"

namespace engine
{

std::vector<std::weak_ptr<IUpdateReceiver>> UpdateManager::m_update_receivers_;
std::vector<std::weak_ptr<IFixedUpdateReceiver>> UpdateManager::m_fixed_update_receivers_;
std::vector<std::weak_ptr<IDrawCallReceiver>> UpdateManager::m_draw_call_receivers_;
std::queue<std::function<void()>> UpdateManager::m_in_cycle_buffer_;

bool UpdateManager::m_in_update_cycle_ = false;
bool UpdateManager::m_in_fixed_update_cycle_ = false;
bool UpdateManager::m_in_draw_call_cycle_ = false;

namespace
{
template <typename It, typename V>
void Subscribe(It &it, V &v)
{
    it.emplace_back(v);
    std::ranges::sort(it, [](const auto &a, const auto &b) {
        return a.lock()->Order() < b.lock()->Order();
    });
}

template <typename It, typename Func>
void Update(It &it, Func func)
{
    auto begin_it = it.begin();
    auto end_it = it.end();
    for (; begin_it != end_it; ++begin_it)
    {
        auto locked = (*begin_it).lock();
        if (locked == nullptr)
        {
            Logger::Warn<UpdateManager>("Expired pointer for %s detected!",
                                        Logger::GetTypeName(typeid(It).name()).c_str());
            continue;
        }

        func(locked);
    }
}

template <typename It, typename V>
void Erase(It &it, V &v)
{
    const auto pos = std::ranges::find_if(it, [&](const auto &r) {
        return r.lock() == v;
    });
    if (pos == it.end())
        return;
    it.erase(pos);
}
}

void UpdateManager::InvokeUpdate()
{
    m_in_update_cycle_ = true;
    Update(m_update_receivers_, [&](const auto &receiver) {
        receiver->OnUpdate();
    });
    m_in_update_cycle_ = false;
    PostFix();
}
void UpdateManager::InvokeFixedUpdate()
{
    m_in_fixed_update_cycle_ = true;
    Update(m_fixed_update_receivers_, [&](const auto &receiver) {
        receiver->OnFixedUpdate();
    });
    m_in_fixed_update_cycle_ = false;
    PostFix();
}
void UpdateManager::InvokeDrawCall()
{
    m_in_draw_call_cycle_ = true;
    Update(m_draw_call_receivers_, [&](const auto &receiver) {
        receiver->OnDraw();
    });
    m_in_draw_call_cycle_ = false;
    PostFix();
}
void UpdateManager::PostFix()
{
    while (!m_in_cycle_buffer_.empty())
    {
        auto func = m_in_cycle_buffer_.front();
        func();
        m_in_cycle_buffer_.pop();
    }
}
void UpdateManager::SubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver)
{
    if (InUpdateCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            SubscribeUpdate(receiver);
        });
        return;
    }

    Subscribe(m_update_receivers_, receiver);
}
void UpdateManager::UnsubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver)
{
    if (InUpdateCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            UnsubscribeUpdate(receiver);
        });
        return;
    }

    Erase(m_update_receivers_, receiver);
}
void UpdateManager::SubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver)
{
    if (InFixedUpdateCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            SubscribeFixedUpdate(receiver);
        });
        return;
    }

    Subscribe(m_fixed_update_receivers_, receiver);
}
void UpdateManager::UnsubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver)
{
    if (InFixedUpdateCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            UnsubscribeFixedUpdate(receiver);
        });
        return;
    }

    Erase(m_fixed_update_receivers_, receiver);
}
void UpdateManager::SubscribeDrawCall(const std::shared_ptr<IDrawCallReceiver> &receiver)
{
    if (InDrawCallCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            SubscribeDrawCall(receiver);
        });
        return;
    }

    Subscribe(m_draw_call_receivers_, receiver);
}
void UpdateManager::UnsubscribeDrawCall(const std::shared_ptr<IDrawCallReceiver> &receiver)
{
    if (InDrawCallCycle())
    {
        m_in_cycle_buffer_.emplace([receiver] {
            UnsubscribeDrawCall(receiver);
        });
        return;
    }

    Erase(m_draw_call_receivers_, receiver);
}
bool UpdateManager::InUpdateCycle()
{
    return m_in_update_cycle_;
}
bool UpdateManager::InFixedUpdateCycle()
{
    return m_in_fixed_update_cycle_;
}
bool UpdateManager::InDrawCallCycle()
{
    return m_in_draw_call_cycle_;
}
}