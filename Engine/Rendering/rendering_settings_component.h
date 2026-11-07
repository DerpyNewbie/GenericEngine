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
    Color m_ambient_color_ = Color(1, 1, 1);
    float m_ambient_intensity_ = 0.0f;

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

        if (version >= 3)
        {
            ar(
                CEREAL_NVP(m_cascade_slices_)
            );
        }

        if (version >= 4)
        {
            ar(
                CEREAL_NVP(m_ambient_color_),
                CEREAL_NVP(m_ambient_intensity_)
            );
        }
    }
};
}
CEREAL_CLASS_VERSION(engine::RenderingSettingsComponent, 4)