#pragma once
#include "light.h"

namespace engine
{
class SpotLight : public Light
{

public:
    void OnAwake() override;
    void OnInspectorGui() override;
    void OnUpdate() override;

    bool InCameraView(const std::array<Vector3, 8> &frustum) override;
    Vector3 GetPos() override;
    int ShadowMapCount() override;
    std::vector<Matrix> CalcViewProj(const std::array<Vector3, 8> &frustum_corners) override;
};
}