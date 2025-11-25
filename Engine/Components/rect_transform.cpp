#include "pch.h"
#include "rect_transform.h"
#include "game_object.h"
#include "gui.h"

namespace engine
{
void RectTransform::OnInspectorGui()
{
    Gui::PropertyField("Anchor Min", anchor_min);
    Gui::PropertyField("Anchor Max", anchor_max);
    Gui::PropertyField("Pivot", pivot);
    Gui::PropertyField("Size Delta", size_delta);
    Gui::PropertyField("Anchored Position", anchored_position);
}

Rect RectTransform::CalculateScreenRect() const
{
    const auto transform = GameObject()->Transform();
    Vector2 parent_size = Vector2::Zero;
    if (const auto parent = std::dynamic_pointer_cast<RectTransform>(transform->Parent()))
    {
        const auto rect = parent->CalculateScreenRect();
        parent_size = rect.size;
    }

    const auto anchor_pos_min = Vector2(anchor_min.x * parent_size.x, anchor_min.y * parent_size.y);
    const auto anchor_pos_max = Vector2(anchor_max.x * parent_size.x, anchor_max.y * parent_size.y);

    const auto stretched_size = anchor_pos_max - anchor_pos_min;

    const auto final_size = stretched_size + size_delta;

    const auto anchored_pos = anchor_pos_min + anchored_position;

    const auto pivot_offset = Vector2(final_size.x * pivot.x, final_size.y * pivot.y);

    const auto screen_pos = anchored_pos - pivot_offset;

    return {screen_pos, final_size};
}
}

CEREAL_REGISTER_TYPE(engine::RectTransform)