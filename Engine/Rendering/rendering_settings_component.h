#pragma once
#include "CabotEngine/Graphics/TextureCube.h"
#include "Components/component.h"

namespace engine
{
class RenderingSettingsComponent final : public Component
{
public:
    static constexpr int kShadowCascadeCount = 3;

private:
    AssetPtr<TextureCube> m_skybox_cube_;
    std::array<float, kShadowCascadeCount> m_cascade_splits_ = {10.0f, 200.0f, 1000.0f};

    bool ShadowCascadeInspector();

public:
    void OnInspectorGui() override;
    void OnStart() override;

    void ApplySettings();

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(m_skybox_cube_));
    }
};
}