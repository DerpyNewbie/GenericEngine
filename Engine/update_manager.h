#pragma once
#include "event_receivers.h"

#include <memory>
#include <vector>

namespace engine
{
class UpdateManager
{
    friend class Engine;

    static std::vector<std::shared_ptr<IUpdateReceiver>> m_update_receivers_;
    static std::vector<std::shared_ptr<IFixedUpdateReceiver>> m_fixed_update_receivers_;
    static std::vector<std::shared_ptr<IDrawCallReceiver>> m_draw_call_receivers_;

    static void InvokeUpdate();
    static void InvokeFixedUpdate();
    static void InvokeDrawCall();

public:
    static void SubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver);
    static void UnsubscribeUpdate(const std::shared_ptr<IUpdateReceiver> &receiver);
    static void SubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver);
    static void UnsubscribeFixedUpdate(const std::shared_ptr<IFixedUpdateReceiver> &receiver);
    static void SubscribeDrawCall(const std::shared_ptr<IDrawCallReceiver> &receiver);
    static void UnsubscribeDrawCall(const std::shared_ptr<IDrawCallReceiver> &receiver);

    static int UpdateCount()
    {
        return static_cast<int>(m_update_receivers_.size());
    }

    static int FixedUpdateCount()
    {
        return static_cast<int>(m_fixed_update_receivers_.size());
    }

    static int DrawCallCount()
    {
        return static_cast<int>(m_draw_call_receivers_.size());
    }
};
}