#pragma once
#include "component.h"
#include "Asset/asset_ptr.h"
#include "Math/rect.h"

namespace engine
{
class RectTransform : public Component
{
    std::weak_ptr<RectTransform> m_parent_ = {};

public:
    Vector2 anchor_min = {0.5f, 0.5f};
    Vector2 anchor_max = {0.5f, 0.5f};
    Vector2 pivot = {0.5f, 0.5f};
    Vector2 size_delta = {100.0f, 100.0f};
    Vector2 anchored_position = {0.0f, 0.0f};

    void OnInspectorGui() override;
    void SetParent(std::shared_ptr<RectTransform> parent);

    [[nodiscard]] Rect CalculateScreenRect(Vector2 canvas_size) const;

};
}