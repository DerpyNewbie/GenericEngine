#pragma once
#include "component.h"

namespace engine
{
class RotatorComponent : public Component
{
    Vector3 m_rotating_axis_ = Vector3::UnitY;
    float m_speed_ = 10;

public:
    void OnUpdate() override;
    void OnInspectorGui() override;
};
}