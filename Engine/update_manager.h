#pragma once
#include "event_receivers.h"

#include <memory>
#include <vector>

namespace engine
{
class UpdateManager
{
    friend class Engine;

    static std::vector<std::weak_ptr<IUpdateReceiver>> m_update_receivers_;
    static std::vector<std::weak_ptr<IFixedUpdateReceiver>> m_fixed_update_receivers_;

    static void InvokeUpdate();
    static void InvokeFixedUpdate();

public:
    // TODO: implement update manager that can be subscribed from anywhere
    static void SubscribeUpdate(std::weak_ptr<IUpdateReceiver> receiver);
    static void UnsubscribeUpdate(std::weak_ptr<IUpdateReceiver> receiver);
    static void SubscribeFixedUpdate(std::weak_ptr<IFixedUpdateReceiver> receiver);
    static void UnsubscribeFixedUpdate(std::weak_ptr<IFixedUpdateReceiver> receiver);
};
}