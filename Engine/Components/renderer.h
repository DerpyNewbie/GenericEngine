#pragma once
#include "component.h"
#include "event_receivers.h"

namespace engine
{
class Renderer : public Component, public IDrawCallReceiver
{
protected:
    bool m_is_visible_ = false;

    void SetVisible(bool visible);

public:
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;
};
}