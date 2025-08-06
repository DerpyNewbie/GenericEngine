#include "pch.h"
#include "renderer_2d.h"
#include "rect_transform.h"
#include "canvas.h"
#include "game_object.h"

namespace engine
{
void Renderer2D::OnEnabled()
{
    canvas = AssetPtr<Canvas>::FromManaged(GameObject()->GetComponentInParent<Canvas>());
    canvas.CastedLock()->AddRenderer(shared_from_base<Renderer2D>());
}

void Renderer2D::OnDisabled()
{
    canvas.CastedLock()->RemoveRenderer(shared_from_base<Renderer2D>());
}

Rect Renderer2D::NormalizedRect()
{
    auto rect_transform = GameObject()->GetComponent<RectTransform>();
    auto rect = rect_transform->CalculateScreenRect();
    auto canvas_size = canvas.CastedLock()->CanvasSize();
    return Rect{
        rect.pos / canvas_size,
        rect.size / canvas_size};
}
}