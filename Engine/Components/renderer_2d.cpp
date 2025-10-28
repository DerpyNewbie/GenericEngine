#include "pch.h"
#include "game_object.h"
#include "Asset/asset_ptr.h"
#include "rect_transform.h"
#include "canvas.h"
#include "renderer_2d.h"

namespace engine
{
void Renderer2D::OnAwake()
{
    GameObject()->AddComponent<RectTransform>();
}

void Renderer2D::OnEnabled()
{
    m_canvas_ = AssetPtr<Canvas>::FromManaged(GameObject()->GetComponentInParent<Canvas>());
    if (auto canvas = m_canvas_.CastedLock())
        canvas->AddRenderer(shared_from_base<Renderer2D>());
}

void Renderer2D::OnDisabled()
{
    if (auto canvas = m_canvas_.CastedLock())
        canvas->RemoveRenderer(shared_from_base<Renderer2D>());
}

void Renderer2D::SetCanvas(const std::shared_ptr<Canvas> &canvas)
{
    m_canvas_ = AssetPtr<Canvas>::FromManaged(canvas);
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