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

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_rotating_axis_),
            CEREAL_NVP(m_speed_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::RotatorComponent, 1)