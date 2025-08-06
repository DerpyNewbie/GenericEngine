#include "pch.h"
#include "rect_transform.h"

#include "engine_util.h"
#include "game_object.h"

namespace engine
{
void RectTransform::OnAwake()
{
    std::shared_ptr<RectTransform> rect_transform = shared_from_base<RectTransform>();
    std::shared_ptr<Transform> transform;

    UINT itr = 0;
    bool find = false;
    for (size_t i = 0; i < GameObject()->m_components_.size(); ++i)
    {
        auto &component = GameObject()->m_components_[i];
        transform = std::dynamic_pointer_cast<Transform>(component);
        if (transform)
        {
            itr = i;
            find = true;
            break;
        }
    }
    if (!find)
    {
        return;
    }

    rect_transform->m_parent_ = transform->Parent();
    for (int i = 0; i < transform->ChildCount(); ++i)
    {
        rect_transform->m_children_[i] = GameObject()->Transform()->GetChild(i);
    }
    GameObject()->m_components_[itr] = rect_transform;

}

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

Rect RectTransform::CalculateScreenRect() const
{
    Vector2 parentSize = Vector2::Zero;
    if (auto parent = std::dynamic_pointer_cast<RectTransform>(Parent()))
    {
        auto rect = parent->CalculateScreenRect();
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

CEREAL_REGISTER_TYPE(engine::RectTransform)