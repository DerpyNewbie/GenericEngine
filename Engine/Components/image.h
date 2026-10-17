#pragma once
#include "renderer_2d.h"
#include "Asset/asset_ptr.h"
#include "Rendering/material.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"

namespace engine
{
/// <summary>
/// Canvas上に画像を描画するRenderer2Dです。
/// </summary>
class Image : public Renderer2D
{
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count> m_world_matrix_buffers_;

    /// <summary>
    /// WorldMatrix用のBufferがなければ作成し、Canvas上の正規化されたRectから作ったMatrixを現在のフレームのBufferに書き込みます。
    /// </summary>
    void UpdateWorldBuffer();
    
public:
    AssetPtr<Material> shared_material;

    void OnInspectorGui() override;
    /// <summary>
    /// WorldMatrixを更新し、QuadのMeshの描画をRenderPipelineに登録します。
    /// </summary>
    void Render() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Renderer2D>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::Image, 1)