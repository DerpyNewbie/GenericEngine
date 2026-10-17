#pragma once
#include "renderer.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
class Material;

/// <summary>
/// 常にCameraの方を向く板(Billboard)を描画するRendererです。
/// </summary>
class BillboardRenderer : public Renderer
{
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count> m_world_matrix_buffers_;

    /// <summary>
    /// WorldMatrix用のBufferがなければ作成し、Main Cameraの方を向くようにしたWorldMatrixを現在のフレームのBufferに書き込みます。
    /// </summary>
    void UpdateWorldBuffer();
    
public:
    /// <summary>
    /// BillboardShaderを使うMaterialを作成し、boundsを初期化します。
    /// </summary>
    void OnConstructed() override;
    void OnInspectorGui() override;
    /// <summary>
    /// WorldMatrixを更新し、QuadのMeshの描画をRenderPipelineに登録します。
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