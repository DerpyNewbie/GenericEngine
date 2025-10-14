#pragma once
#include "renderer.h"
#include "Asset/asset_ptr.h"
#include "Rendering/font_data.h"

namespace engine
{

class TextRenderer : public Renderer
{
public:
    Vector2 position;
    AssetPtr<FontData> font_data;
    std::string string;
    Color color;

    void OnInspectorGui() override;
    void Render() override;

    std::shared_ptr<Transform> BoundsOrigin() override;
};
}