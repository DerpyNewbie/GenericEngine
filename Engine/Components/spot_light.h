#pragma once
#include "light.h"

namespace engine
{
class SpotLight : public Light
{

public:
    void OnConstructed() override;
    void OnInspectorGui() override;

    void UpdateData() override;
    bool InCameraView(const std::array<Vector3, 8> &frustum) override;
    Vector3 GetPos() override;
    int ShadowMapCount() override;
    std::vector<Matrix> CalcViewProj(const std::array<Vector3, 8> &frustum_corners) override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_light_data_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::SpotLight, 1)