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
template <typename It, typename Func>
void CustomUpdate(It begin_it, const It &end_it, Func func)
{
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
}

void UpdateManager::InvokeUpdate()
{
    CustomUpdate(m_update_receivers_.begin(), m_update_receivers_.end(), [&](const auto &receiver) {
        receiver->OnUpdate();
    });
}
void UpdateManager::InvokeFixedUpdate()
{
    CustomUpdate(m_fixed_update_receivers_.begin(), m_fixed_update_receivers_.end(), [&](const auto &receiver) {
        receiver->OnFixedUpdate();
    });
}
void UpdateManager::InvokeDrawCall()
{
    CustomUpdate(m_draw_call_receivers_.begin(), m_draw_call_receivers_.end(), [&](const auto &receiver) {
        receiver->OnDraw();
    });
}
void UpdateManager::SubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver)
{
    m_update_receivers_.emplace_back(receiver);
    std::ranges::sort(m_update_receivers_, [](const auto &a, const auto &b) {
        return a.lock()->Order() < b.lock()->Order();
    });
}
void UpdateManager::UnsubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver)
{
    m_update_receivers_.erase(std::ranges::find_if(m_update_receivers_, [&](const auto &r) {
        return r.lock() == receiver;
    }));
}
void UpdateManager::SubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver)
{
    m_fixed_update_receivers_.emplace_back(receiver);
    std::ranges::sort(m_fixed_update_receivers_, [](const auto &a, const auto &b) {
        return a.lock()->Order() < b.lock()->Order();
    });
}
void UpdateManager::UnsubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver)
{
    m_fixed_update_receivers_.erase(std::ranges::find_if(m_fixed_update_receivers_, [&](const auto &r) {
        return r.lock() == receiver;
    }));
}
void UpdateManager::SubscribeDrawCall(const std::shared_ptr<IDrawCallReceiver> &receiver)
{
    m_draw_call_receivers_.emplace_back(receiver);
    std::ranges::sort(m_draw_call_receivers_, [](const auto &a, const auto &b) {
        return a.lock()->Order() < b.lock()->Order();
    });
}
void UpdateManager::UnsubscribeDrawCall(const std::shared_ptr<IDrawCallReceiver> &receiver)
{
    m_draw_call_receivers_.erase(std::ranges::find_if(m_draw_call_receivers_, [&](const auto &r) {
        return r.lock() == receiver;
    }));
}
}