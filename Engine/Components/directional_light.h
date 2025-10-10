#pragma once
#include "light.h"
#include "Rendering/rendering_settings_component.h"

namespace engine
{
class DirectionalLight : public Light
{
    friend class RenderPipeline;
    static std::array<float, RenderingSettingsComponent::kShadowCascadeCount> m_cascade_slices_;

    static void CascadeFrustum(const std::array<Vector3, 8> &frustum,
                               std::array<std::array<Vector3, 8>, RenderingSettingsComponent::kShadowCascadeCount> &
                               dst);

public:
    static void SetCascadeSlices(
        std::array<float, RenderingSettingsComponent::kShadowCascadeCount> shadow_cascade_sprits);

    void OnInspectorGui() override;
    void OnUpdate() override;

    bool InCameraView(const std::array<Vector3, 8> &frustum) override;
    int ShadowMapCount() override;
    std::vector<Matrix> CalcViewProj(const std::array<Vector3, 8> &frustum_corners) override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Light>(this));
    }
};
}