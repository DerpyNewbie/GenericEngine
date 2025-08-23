#pragma once
#include "Components/component.h"

namespace engine
{
enum class kTestType : unsigned char
{
    kStarting,
    kForce,
    kImpulse,
    kForceAtPosition
};

class RigidbodyTesterComponent : public Component
{
    kTestType m_test_type_ = kTestType::kStarting;
    Vector3 m_force_ = {10, 0, 0};
    Vector3 m_position_ = {0, 0, 0};

public:
    void OnFixedUpdate() override;
    void OnInspectorGui() override;
};
}