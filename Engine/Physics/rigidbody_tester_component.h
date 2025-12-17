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

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_test_type_),
            CEREAL_NVP(m_force_),
            CEREAL_NVP(m_position_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::RigidbodyTesterComponent, 1)