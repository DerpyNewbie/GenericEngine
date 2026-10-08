#pragma once
#include "rendering_constants.h"
#include "CabotEngine/Graphics/TextureCube.h"
#include "Components/component.h"

namespace engine
{
/// <summary>
/// SkyboxやShadowのCascadeなど、描画の設定を行うComponentです。
/// </summary>
class RenderingSettingsComponent final : public Component
{
    AssetPtr<TextureCube> m_skybox_cube_;
    std::array<float, RenderingConstants::kShadowCascadeCount> m_cascade_slices_ = {10.0f, 200.0f, 1000.0f};

    /// <summary>
    /// Cascadeの区切りを編集するGuiを表示します。
    /// </summary>
    /// <returns>値が変更された場合 true</returns>
    bool ShadowCascadeInspector();

public:
    /// <summary>
    /// SkyboxのTextureとCascadeの区切りをInspectorに表示し、変更された場合は設定を適用します。
    /// </summary>
    void OnInspectorGui() override;
    /// <summary>
    /// 設定を適用します。
    /// </summary>
    void OnAwake() override;

    /// <summary>
    /// SkyboxのTextureとCascadeの区切りを、Skybox、Lighting、DirectionalLightに反映します。
    /// </summary>
    void ApplySettings() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_skybox_cube_)
            );
    }
};
}