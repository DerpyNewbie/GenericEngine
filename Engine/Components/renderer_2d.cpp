#include "pch.h"
#include "renderer_2d.h"
#include "rect_transform.h"
#include "canvas.h"
#include "game_object.h"

namespace engine
{
void Renderer2D::OnAwake()
{
    GameObject()->AddComponent<RectTransform>();
}
void Renderer2D::OnUpdate()
{
    if (auto canvas = m_canvas_.Lock())
    {
        return;
    }

    m_canvas_ = AssetPtr<Canvas>::FromManaged(GameObject()->GetComponentInParent<Canvas>());
    auto canvas = m_canvas_.CastedLock();
    if (!canvas && GameObject()->IsActiveInHierarchy())
        return;
    canvas->AddRenderer(shared_from_base<Renderer2D>());
}

void Renderer2D::OnDisabled()
{
    m_canvas_.CastedLock()->RemoveRenderer(shared_from_base<Renderer2D>());
}

Rect Renderer2D::NormalizedRect()
{
    auto canvas = m_canvas_.CastedLock();
    if (!canvas)
    {
        return Rect{Vector2::Zero,
                    Vector2::Zero};
    }

    auto canvas_size = m_canvas_.CastedLock()->CanvasSize();
    auto rect_transform = GameObject()->GetComponent<RectTransform>();
    auto rect = rect_transform->CalculateScreenRect();
    return Rect{
        rect.pos / canvas_size,
        rect.size / canvas_size};
}
}

CEREAL_REGISTER_TYPE(engine::Renderer2D)