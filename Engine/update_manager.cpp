#include "update_manager.h"

#include "logger.h"

namespace engine
{

std::vector<std::shared_ptr<IUpdateReceiver>> UpdateManager::m_update_receivers_;
std::vector<std::shared_ptr<IFixedUpdateReceiver>> UpdateManager::m_fixed_update_receivers_;
std::vector<std::shared_ptr<IDrawCallReceiver>> UpdateManager::m_draw_call_receivers_;

template <typename It, typename Func>
void CustomUpdate(It begin_it, It end_it, Func func)
{
    for (; begin_it != end_it; ++begin_it)
    {
        if (*begin_it == nullptr)
        {
            Logger::Warn<UpdateManager>("[Update] Expired pointer for %s detected!",
                                        Logger::GetTypeName(typeid(It).name()));
            continue;
        }
    
        func(*begin_it);
    }
}

void UpdateManager::InvokeUpdate()
{
    CustomUpdate(
        m_update_receivers_.begin(), m_update_receivers_.end(),
        [&](auto receiver) {
            receiver->OnUpdate();
        });
}
void UpdateManager::InvokeFixedUpdate()
{
    CustomUpdate(
        m_fixed_update_receivers_.begin(), m_fixed_update_receivers_.end(),
        [&](auto receiver) {
            receiver->OnFixedUpdate();
        });
}
void UpdateManager::InvokeDrawCall()
{
    CustomUpdate(
        m_draw_call_receivers_.begin(), m_draw_call_receivers_.end(),
        [&](auto receiver) {
            receiver->OnDraw();
        });
}
void UpdateManager::SubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver)
{
    m_update_receivers_.push_back(receiver);
}
void UpdateManager::UnsubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver)
{
    m_update_receivers_.erase(
        std::find(m_update_receivers_.begin(), m_update_receivers_.end(), receiver));
}
void UpdateManager::SubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver)
{
    m_fixed_update_receivers_.push_back(receiver);
}
void UpdateManager::UnsubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver)
{
    m_fixed_update_receivers_.erase(
        std::find(m_fixed_update_receivers_.begin(), m_fixed_update_receivers_.end(), receiver));
}
void UpdateManager::SubscribeDrawCall(const std::shared_ptr<IDrawCallReceiver> &receiver)
{
    m_draw_call_receivers_.push_back(receiver);
}
void UpdateManager::UnsubscribeDrawCall(const std::shared_ptr<IDrawCallReceiver> &receiver)
{
    m_draw_call_receivers_.erase(
        std::find(m_draw_call_receivers_.begin(), m_draw_call_receivers_.end(), receiver));
}
}