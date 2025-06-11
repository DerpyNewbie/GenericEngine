#pragma once
#include "component.h"

namespace engine
{
class Controller : public Component
{
    VECTOR m_rotation_ = {};

public:
    void OnUpdate() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(m_rotation_));
    }
};
}