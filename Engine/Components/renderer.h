#pragma once
#include "component.h"
#include "event_receivers.h"

namespace engine
{
class Renderer : public Component, public IDrawCallReceiver
{
public:
    void OnAwake() override;
    void OnDestroy() override;
};
}