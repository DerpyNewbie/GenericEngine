#pragma once
#include "light.h"
#include "Rendering/rendering_constants.h"

namespace engine
{
/// <summary>
/// 一方向から照らす平行光源のLightです。
/// </summary>
class DirectionalLight : public Light
{
    friend class RenderPipeline;
    static std::array<float, RenderingConstants::kShadowCascadeCount> m_cascade_slices_;

    /// <summary>
    /// カメラのFrustumを、Cascadeの区切りに従ってCascadeの数だけ分割します。
    /// </summary>
    /// <param name="frustum">カメラのFrustumの8頂点</param>
    /// <param name="dst">分割されたFrustumの8頂点の書き込み先</param>
    static void CascadeFrustum(
        const std::array<Vector3, 8> &frustum,
        std::array<std::array<Vector3, 8>, RenderingConstants::kShadowCascadeCount> &
        dst
    );

public:
    /// <summary>
    /// Cascadeの区切りとなる距離を設定します。
    /// </summary>
    /// <param name="shadow_cascade_slices">各Cascadeの遠い側の距離</param>
    static void SetCascadeSlices(
        std::array<float, RenderingConstants::kShadowCascadeCount> shadow_cascade_slices
    );

    /// <summary>
    /// LightDataの種類をDirectionalに設定します。
    /// </summary>
    void OnConstructed() override;
    void OnInspectorGui() override;
    /// <summary>
    /// Transformの前方向をLightDataのdirectionに反映します。
    /// </summary>
    void UpdateData() override;
    /// <summary>
    /// 常に true を返します。DirectionalLightは常にカメラに影響します。
    /// </summary>
    bool InCameraView(const std::array<Vector3, 8> &frustum) override;
    /// <summary>
    /// Main Cameraの位置を返します。
    /// </summary>
    Vector3 GetPos() override;
    /// <summary>
    /// 使用するShadowMapの枚数(3)を返します。
    /// </summary>
    int ShadowMapCount() override;
    /// <summary>
    /// Cascadeごとに、分割したFrustumを覆う平行投影のViewProjectionMatrixを計算します。
    /// </summary>
    /// <param name="frustum_corners">カメラのFrustumの8頂点</param>
    /// <returns>CascadeごとのViewProjectionMatrix</returns>
    std::vector<Matrix> CalcViewProj(const std::array<Vector3, 8> &frustum_corners) override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Light>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::DirectionalLight, 1)