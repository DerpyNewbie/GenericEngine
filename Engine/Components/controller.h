#pragma once
#include "component.h"

namespace engine
{
class Controller : public Component
{
    VECTOR m_rotation_ = {};
public:
    void OnAwake() override
    {}

    void OnStart() override
    {}
    void OnUpdate() override;
};
}