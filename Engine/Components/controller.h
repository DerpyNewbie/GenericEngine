#pragma once
#include "component.h"

namespace engine
{
class Controller : public Component
{
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