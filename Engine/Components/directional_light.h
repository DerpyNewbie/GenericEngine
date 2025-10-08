#pragma once
#include "light.h"
#include "Rendering/rendering_settings_component.h"

namespace engine
{
class DirectionalLight : public Light
{
    friend class RenderPipeline;
    static std::array<float, RenderingSettingsComponent::kShadowCascadeCount> m_cascade_sprits_;

    static std::array<std::array<Vector3, 8>, RenderingSettingsComponent::kShadowCascadeCount> &CascadeFrustum(
        const std::array<Vector3, 8> &frustum);

public:
    static void SetCascadeSprits(
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