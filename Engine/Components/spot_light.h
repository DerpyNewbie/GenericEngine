#pragma once
#include "light.h"

namespace engine
{
/// <summary>
/// 円錐状に照らすSpotLightです。
/// </summary>
class SpotLight : public Light
{

public:
    /// <summary>
    /// LightDataの種類をSpotLightに設定し、内側 / 外側の角度を初期化します。
    /// </summary>
    void OnConstructed() override;
    void OnInspectorGui() override;

    /// <summary>
    /// Transformの位置と前方向をLightDataに反映します。
    /// </summary>
    void UpdateData() override;
    /// <summary>
    /// 照射範囲を球で近似し、カメラのFrustumを囲むAABBと重なっているかどうかを判定します。
    /// </summary>
    /// <param name="frustum">カメラのFrustumの8頂点</param>
    bool InCameraView(const std::array<Vector3, 8> &frustum) override;
    /// <summary>
    /// Transformの位置を返します。
    /// </summary>
    Vector3 GetPos() override;
    /// <summary>
    /// 使用するShadowMapの枚数(1)を返します。
    /// </summary>
    int ShadowMapCount() override;
    /// <summary>
    /// Lightの位置と向き、外側の角度、Rangeから、透視投影のViewProjectionMatrixを計算します。
    /// </summary>
    /// <param name="frustum_corners">カメラのFrustumの8頂点(使われていません)</param>
    /// <returns>ViewProjectionMatrix(1つ)</returns>
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