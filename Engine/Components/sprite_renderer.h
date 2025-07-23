#pragma once
#include "pch.h"
#include "renderer.h"
#include "Asset/asset_ptr.h"
#include "Rendering/FontData.h"

namespace engine
{

class SpriteRenderer : public Renderer
{
public:
    Vector2 position;
    AssetPtr<FontData> font_data;
    std::string string;
    Color color;

    void OnInspectorGui() override;
    void OnDraw() override;

    std::weak_ptr<Transform> GetTransform() override;
};
}