#pragma once
#include "renderer_2d.h"
#include "Asset/asset_ptr.h"
#include "Rendering/material.h"
#include "Rendering/texture2d.h"

namespace engine
{
/// <summary>
/// Canvas上に画像を描画するRenderer2Dです。
/// </summary>
class Image : public Renderer2D
{
    /// <summary>
    /// Canvas上の正規化されたRectから、Quadを画面に置くMatrixを作ります。
    /// </summary>
    Matrix CalcScreenMatrix() const;

public:
    AssetPtr<Material> shared_material;

    void OnInspectorGui() override;
    /// <summary>
    /// MaterialのTexture(_MainTex)を貼った四角形を、CaboRenderingSystemのUIとして登録します。
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