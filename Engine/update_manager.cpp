#include "pch.h"

#include "update_manager.h"

#include "logger.h"

namespace engine
{

std::vector<std::weak_ptr<IUpdateReceiver>> UpdateManager::m_update_receivers_;
std::vector<std::weak_ptr<IFixedUpdateReceiver>> UpdateManager::m_fixed_update_receivers_;
std::vector<std::weak_ptr<IDrawCallReceiver>> UpdateManager::m_draw_call_receivers_;

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
    Update(m_update_receivers_, [&](const auto &receiver) {
        receiver->OnUpdate();
    });
}
void UpdateManager::InvokeFixedUpdate()
{
    Update(m_fixed_update_receivers_, [&](const auto &receiver) {
        receiver->OnFixedUpdate();
    });
}
void UpdateManager::InvokeDrawCall()
{
    Update(m_draw_call_receivers_, [&](const auto &receiver) {
        receiver->OnDraw();
    });
}
void UpdateManager::SubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver)
{
    Subscribe(m_update_receivers_, receiver);
}
void UpdateManager::UnsubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver)
{
    Erase(m_update_receivers_, receiver);
}
void UpdateManager::SubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver)
{
    Subscribe(m_fixed_update_receivers_, receiver);
}
void UpdateManager::UnsubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver)
{
    Erase(m_fixed_update_receivers_, receiver);
}
void UpdateManager::SubscribeDrawCall(const std::shared_ptr<IDrawCallReceiver> &receiver)
{
    Subscribe(m_draw_call_receivers_, receiver);
}
void UpdateManager::UnsubscribeDrawCall(const std::shared_ptr<IDrawCallReceiver> &receiver)
{
    Erase(m_draw_call_receivers_, receiver);
}
}