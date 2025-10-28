#pragma once
#include "light.h"

namespace engine
{
class SpotLight : public Light
{

public:
    void OnConstructed() override;
    void OnInspectorGui() override;
    void OnUpdate() override;

    bool InCameraView(const std::array<Vector3, 8> &frustum) override;
    Vector3 GetPos() override;
    int ShadowMapCount() override;
    std::vector<Matrix> CalcViewProj(const std::array<Vector3, 8> &frustum_corners) override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_light_data_));
    }
};
}