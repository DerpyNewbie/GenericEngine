#include "pch.h"
#include "rect_transform.h"

#include "engine_util.h"

namespace engine
{
void RectTransform::OnInspectorGui()
{
    float anc_min[2], anc_max[2];
    float piv[2], size_del[2], anc_pos[2];
    EngineUtil::ToFloat2(anc_min, anchor_min);
    EngineUtil::ToFloat2(anc_max, anchor_max);
    EngineUtil::ToFloat2(piv, pivot);
    EngineUtil::ToFloat2(size_del, size_delta);
    EngineUtil::ToFloat2(anc_pos, anchored_position);

    if (ImGui::InputFloat2("anchor_min", anc_min))
    {
        anchor_min.x = anc_min[0];
        anchor_min.y = anc_min[1];
    }
    if (ImGui::InputFloat2("anchor_max", anc_max))
    {
        anchor_max.x = anc_max[0];
        anchor_max.y = anc_max[1];
    }
    if (ImGui::InputFloat2("pivot", piv))
    {
        pivot.x = piv[0];
        pivot.y = piv[1];
    }
    if (ImGui::InputFloat2("size_delta", size_del))
    {
        size_delta.x = size_del[0];
        size_delta.y = size_del[1];
    }
    if (ImGui::InputFloat2("anchored_position", anc_max))
    {
        anchor_max.x = anc_max[0];
        anchor_max.y = anc_max[1];
    }
}

void RectTransform::SetParent(std::shared_ptr<RectTransform> parent)
{
    m_parent_ = parent;
}

Rect RectTransform::CalculateScreenRect(Vector2 canvas_size) const
{
    Vector2 parentSize = canvas_size;
    if (auto parent = m_parent_.lock())
    {
        auto rect = parent->CalculateScreenRect(canvas_size);
        parentSize = rect.size;
    }

    const auto anchor_pos_min = Vector2(anchor_min.x * parentSize.x, anchor_min.y * parentSize.y);
    const auto anchor_pos_max = Vector2(anchor_max.x * parentSize.x, anchor_max.y * parentSize.y);

    const auto stretched_size = anchor_pos_max - anchor_pos_min;

    const auto final_size = stretched_size + size_delta;

    const auto anchored_pos = anchor_pos_min + anchored_position;

    const auto pivotOffset = Vector2(final_size.x * pivot.x, final_size.y * pivot.y);

    const auto screen_pos = anchored_pos - pivotOffset;

    return {screen_pos, final_size};
}
}