#pragma once
#include "renderer.h"
#include "Rendering/CaboRenderingSystem/Scene/mesh_renderer_data_store.h"

namespace engine
{
class Material;

/// <summary>
/// 常にCameraの方を向く板(Billboard)を描画するRendererです。
/// </summary>
class BillboardRenderer : public Renderer
{
    // CaboRenderingSystemに登録した描画データのID
    CRS::MeshRendererId m_crs_id_;

    /// <summary>
    /// 非表示になった時に、CaboRenderingSystemから描画データを取り除きます。
    /// </summary>
    void OnVisibilityChanged(bool visible) override;

public:
    /// <summary>
    /// Unlit.hlslを使うMaterialを作成し、boundsを初期化します。
    /// </summary>
    void OnConstructed() override;
    void OnInspectorGui() override;
    /// <summary>
    /// Main Cameraの方を向くようにしたWorldMatrixで、QuadのMeshの描画データをCaboRenderingSystemに登録 / 更新します。
    /// </summary>
    void Render() override;
    /// <summary>
    /// boundsの基準となるMatrixとして、自身のWorldMatrixを返します。
    /// </summary>
    Matrix BoundsOrigin() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Renderer>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::BillboardRenderer, 1)