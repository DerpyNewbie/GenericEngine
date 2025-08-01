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
    void OnDraw() override;

    std::weak_ptr<Transform> BoundsOrigin() override;
};
}