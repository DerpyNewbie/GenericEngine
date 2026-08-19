#pragma once
#include "rendering_constants.h"
#include "CabotEngine/Graphics/TextureCube.h"
#include "Components/component.h"

namespace engine
{
class RenderingSettingsComponent final : public Component
{
    AssetPtr<TextureCube> m_skybox_cube_;
    std::array<float, RenderingConstants::kShadowCascadeCount> m_cascade_slices_ = {10.0f, 200.0f, 1000.0f};
    uint16_t m_effect_render_queue_ = 5500;

    bool ShadowCascadeInspector();

public:
    void OnInspectorGui() override;
    void OnAwake() override;

    void ApplySettings();
    
    template <class Archive>
    void serialize(Archive& ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_skybox_cube_)
        );

        if (version >= 2)
        {
            ar(
                CEREAL_NVP(m_effect_render_queue_)
            );
        }
    }
};
}
CEREAL_CLASS_VERSION(engine::RenderingSettingsComponent, 2)