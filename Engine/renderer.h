#pragma once
#include "component.h"
#include "event_receivers.h"

namespace engine
{
class Renderer : public Component, public IDrawCallReceiver
{
    void OnAwake() override;
};
}