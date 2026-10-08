#pragma once
#include "transform.h"
#include "Math/rect.h"

namespace engine
{
/// <summary>
/// Canvas上での位置とサイズを、AnchorとPivotで指定するComponentです。
/// </summary>
class RectTransform : public Component
{
public:
    Vector2 anchor_min = {0.5f, 0.5f};
    Vector2 anchor_max = {0.5f, 0.5f};
    Vector2 pivot = {0.5f, 0.5f};
    Vector2 size_delta = {100.0f, 100.0f};
    Vector2 anchored_position = {0.0f, 0.0f};

    void OnInspectorGui() override;

    /// <summary>
    /// 親のRectTransformのサイズ、Anchor、Pivot、SizeDelta、AnchoredPositionから、位置とサイズを計算します。
    /// </summary>
    [[nodiscard]] Rect CalculateScreenRect() const;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(anchor_min),
            CEREAL_NVP(anchor_max),
            CEREAL_NVP(pivot),
            CEREAL_NVP(size_delta),
            CEREAL_NVP(anchored_position)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::RectTransform, 1)