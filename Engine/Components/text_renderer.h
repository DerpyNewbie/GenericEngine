#pragma once
#include "renderer.h"
#include "Asset/asset_ptr.h"
#include "Rendering/font_data.h"

namespace engine
{

/// <summary>
/// 文字列を描画するRendererです。
/// </summary>
class TextRenderer : public Renderer
{
public:
    Vector2 position;
    AssetPtr<FontData> font_data;
    std::string string;
    Color color;

    void OnInspectorGui() override;
    /// <summary>
    /// 文字列の描画をRenderPipelineに登録します。
    /// </summary>
    void Render() override;

    /// <summary>
    /// boundsの基準となるMatrixとして、現在描画中のCameraのWorldMatrixを返します。
    /// </summary>
    Matrix BoundsOrigin() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Renderer>(this),
            CEREAL_NVP(position),
            CEREAL_NVP(font_data),
            CEREAL_NVP(color),
            CEREAL_NVP(string)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::TextRenderer, 1)