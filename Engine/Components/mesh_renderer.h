#pragma once
#include "component.h"
#include "renderer.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"
#include "Rendering/CaboRenderingSystem/Scene/mesh_renderer_data_store.h"

namespace engine
{
/// <summary>
/// Meshを描画するRendererです。描画データはCaboRenderingSystemに登録します。
/// </summary>
class MeshRenderer : public Renderer
{
    /// <summary>
    /// boundsの基準となるMatrixとして、自身のWorldMatrixを返します。
    /// </summary>
    Matrix BoundsOrigin() override;

protected:
    static bool m_draw_bounds_;

    AssetPtr<Mesh> m_shared_mesh_;

    // CaboRenderingSystemに登録した描画データのID
    CRS::MeshRendererId m_crs_id_;

    /// <summary>
    /// 非表示になった時に、CaboRenderingSystemから描画データを取り除きます。
    /// </summary>
    void OnVisibilityChanged(bool visible) override;
    /// <summary>
    /// Meshの描画データをCaboRenderingSystemに登録 / 更新します。
    /// </summary>
    /// <param name="world">WorldMatrix</param>
    /// <param name="local_bounds">worldを基準としたbounds</param>
    /// <param name="bone_buffer">スキニングに使うボーン行列のBuffer。使わない場合は無効なハンドル</param>
    void SubmitToRenderer(const Matrix &world, const DirectX::BoundingBox &local_bounds,
                          CRS::BufferHandle bone_buffer = {});

    /// <summary>
    /// boundsをGizmosで描画します。
    /// </summary>
    void DrawBounds();
    /// <summary>
    /// Meshの頂点からboundsを計算し直します。
    /// </summary>
    void RecalculateBoundingBox();

public:

    bool buffer_creation_failed = false;

    void OnInspectorGui() override;
    /// <summary>
    /// Meshの描画データをCaboRenderingSystemに登録 / 更新します。
    /// </summary>
    void Render() override;

    /// <summary>
    /// 描画するMeshを設定し、boundsを計算し直します。
    /// </summary>
    /// <param name="mesh">描画するMesh</param>
    void SetSharedMesh(const AssetPtr<Mesh> &mesh);

    /// <summary>
    /// 描画するMeshを取得します。
    /// </summary>
    AssetPtr<Mesh> GetSharedMesh()
    {
        return m_shared_mesh_;
    }

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Renderer>(this),
            CEREAL_NVP(m_shared_mesh_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::MeshRenderer, 1)
