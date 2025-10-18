#pragma once
#include "transform.h"
#include "Math/rect.h"

namespace engine
{
class RectTransform : public Transform
{
public:
    Vector2 anchor_min = {0.5f, 0.5f};
    Vector2 anchor_max = {0.5f, 0.5f};
    Vector2 pivot = {0.5f, 0.5f};
    Vector2 size_delta = {100.0f, 100.0f};
    Vector2 anchored_position = {0.0f, 0.0f};

    void OnAwake() override;
    void OnInspectorGui() override;

    [[nodiscard]] Rect CalculateScreenRect() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Transform>(this),
           CEREAL_NVP(anchor_min),
           CEREAL_NVP(anchor_max),
           CEREAL_NVP(pivot),
           CEREAL_NVP(size_delta),
           CEREAL_NVP(anchored_position));
    }
};
}