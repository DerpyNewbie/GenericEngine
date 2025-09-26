#pragma once
#include "light.h"

namespace engine
{
class DirectionalLight : public Light
{
    float m_cover_size_ = 10.0f;

public:
    void OnInspectorGui() override;
    void OnUpdate() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_light_data_),
           CEREAL_NVP(m_cover_size_));
    }
};
}