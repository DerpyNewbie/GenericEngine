#pragma once
#include "component.h"

namespace engine
{
class Controller : public Component
{
    float m_movement_speed_ = 100.0f;
    float m_rotation_speed_ = 2.0f;

    Vector3 m_rotation_ = {};

    Vector3 m_last_movement_input_;
    Vector2 m_last_rotation_input_;

public:
    void OnUpdate() override;
    void OnInspectorGui() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(m_rotation_));
    }
};
}