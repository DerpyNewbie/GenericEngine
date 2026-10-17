#pragma once
#include "component.h"
#include "Rendering/light_data.h"
#include "Rendering/render_pipeline.h"
#include "Rendering/CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
/// <summary>
/// Lightの種類です。
/// </summary>
enum class kLightType : uint8_t
{
    kDirectional,
    kSpotLight,
};

/// <summary>
/// すべてのLightの基底クラスです。
/// </summary>
class Light : public Component
{
    friend class CameraComponent;
    friend class RenderPipeline;
    friend class Lighting;

protected:
    std::vector<int> m_depth_texture_handle_;
    LightData m_light_data_;
    bool m_has_shadow_;

public:
    void OnInspectorGui() override;
    /// <summary>
    /// Lightingに自身を登録し、影が有効な場合はShadowMapの割り当てを試みます。
    /// </summary>
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    /// <summary>
    /// Transformの状態をLightDataに反映します。
    /// </summary>
    virtual void UpdateData() = 0;
    /// <summary>
    /// Lightの影響範囲がカメラのFrustumに入っているかどうかを判定します。
    /// </summary>
    /// <param name="frustum">カメラのFrustumの8頂点</param>
    virtual bool InCameraView(const std::array<Vector3, 8> &frustum) = 0;
    /// <summary>
    /// Lightの位置を取得します。
    /// </summary>
    virtual Vector3 GetPos() = 0;
    /// <summary>
    /// このLightが使用するShadowMapの枚数を取得します。
    /// </summary>
    virtual int ShadowMapCount() = 0;
    /// <summary>
    /// ShadowMapの描画に使うViewProjectionMatrixを計算します。
    /// </summary>
    /// <param name="frustum_corners">カメラのFrustumの8頂点</param>
    /// <returns>ShadowMapごとのViewProjectionMatrix</returns>
    virtual std::vector<Matrix> CalcViewProj(const std::array<Vector3, 8> &frustum_corners) = 0;

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

CEREAL_CLASS_VERSION(engine::Light, 1)