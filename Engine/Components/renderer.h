#pragma once
#include "component.h"
#include "event_receivers.h"

namespace engine
{
class Renderer : public Component, public IDrawCallReceiver
{
    static std::vector<std::weak_ptr<IDrawCallReceiver>> m_draw_call_receivers_;

protected:
    bool m_is_visible_ = false;

    void SetVisible(bool visible);

public:
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    friend class Camera;
};
}