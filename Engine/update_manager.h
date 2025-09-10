#pragma once
#include "event_receivers.h"

namespace engine
{
class UpdateManager
{
    friend class Engine;

    static std::vector<std::weak_ptr<IUpdateReceiver>> m_update_receivers_;
    static std::vector<std::weak_ptr<IFixedUpdateReceiver>> m_fixed_update_receivers_;
    static std::queue<std::function<void()>> m_in_cycle_buffer_;

    static bool m_in_update_cycle_;
    static bool m_in_fixed_update_cycle_;

    static void InvokeUpdate();
    static void InvokeFixedUpdate();
    static void PostFix();

public:
    static void SubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver);
    static void UnsubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver);
    static void SubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver);
    static void UnsubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver);

    static int UpdateCount()
    {
        return static_cast<int>(m_update_receivers_.size());
    }

    static int FixedUpdateCount()
    {
        return static_cast<int>(m_fixed_update_receivers_.size());
    }

    static const std::vector<std::weak_ptr<IUpdateReceiver>> &GetUpdateReceivers()
    {
        return m_update_receivers_;
    }

    static const std::vector<std::weak_ptr<IFixedUpdateReceiver>> &GetFixedUpdateReceivers()
    {
        return m_fixed_update_receivers_;
    }

    static bool InUpdateCycle();
    static bool InFixedUpdateCycle();
};
}