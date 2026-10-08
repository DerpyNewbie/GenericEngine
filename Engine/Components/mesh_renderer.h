#pragma once
#include "component.h"
#include "renderer.h"
#include "Rendering/material_data.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
/// <summary>
/// Meshを描画するRendererです。
/// </summary>
class MeshRenderer : public Renderer
{
    /// <summary>
    /// boundsの基準となるMatrixとして、自身のWorldMatrixを返します。
    /// </summary>
    Matrix BoundsOrigin() override;
    /// <summary>
    /// WorldMatrix用のBufferがなければ作成し、自身のWorldMatrixを現在のフレームのBufferに書き込みます。
    /// </summary>
    virtual void UpdateWorldBuffer();

protected:
    static bool m_draw_bounds_;
    
    AssetPtr<Mesh> m_shared_mesh_;

    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count> m_world_matrix_buffers_;

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
    /// ShadowMap用に、Materialを使わずWorldMatrixのみを設定してMeshを描画します。
    /// </summary>
    void DepthRender() override;
    /// <summary>
    /// WorldMatrixを更新し、Meshの描画をRenderPipelineに登録します。
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