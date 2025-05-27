#pragma once
#include "component.h"

namespace engine
{
class Controller : public Component
{
    VECTOR m_rotation_ = {};

public:
    void OnUpdate() override;
};
}