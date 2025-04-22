#include "update_manager.h"

#include "logger.h"

namespace engine
{

std::vector<std::weak_ptr<IUpdateReceiver>> UpdateManager::m_update_receivers_;
std::vector<std::weak_ptr<IFixedUpdateReceiver>> UpdateManager::m_fixed_update_receivers_;

void UpdateManager::InvokeUpdate()
{
    auto it = m_update_receivers_.begin();
    while (it != m_update_receivers_.end())
    {
        if (it->expired())
        {
            Logger::Warn<UpdateManager>("[Update] Expired IUpdateReceiver pointer detected!");
            ++it;
            continue;
        }

        auto ptr = it->lock();
        if (ptr == nullptr)
        {
            Logger::Warn<UpdateManager>("[Update] Destroyed IUpdateReceiver pointer detected!");
            ++it;
            continue;
        }

        ptr->OnUpdate();
    }
}
void UpdateManager::InvokeFixedUpdate()
{}
void UpdateManager::SubscribeUpdate(std::weak_ptr<IUpdateReceiver> receiver)
{}
void UpdateManager::UnsubscribeUpdate(std::weak_ptr<IUpdateReceiver> receiver)
{}
void UpdateManager::SubscribeFixedUpdate(std::weak_ptr<IFixedUpdateReceiver> receiver)
{}
void UpdateManager::UnsubscribeFixedUpdate(std::weak_ptr<IFixedUpdateReceiver> receiver)
{}
}