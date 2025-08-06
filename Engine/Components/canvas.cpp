#include "pch.h"
#include "Components/renderer_2d.h"
#include "canvas.h"
#include "gui.h"

namespace engine
{
void Canvas::OnInspectorGui()
{
    float canvas_size[2];
    EngineUtil::ToFloat2(canvas_size, m_canvas_size_);
    if (ImGui::InputFloat2("CanvasSize", canvas_size))
    {
        m_canvas_size_.x = canvas_size[0];
        m_canvas_size_.y = canvas_size[1];
    }

    Gui::PropertyField("TargetCamera", m_target_camera_);
}

void Canvas::OnDraw()
{
    for (auto it : m_children_renderers_)
        it->Render();
}

Vector2 Canvas::CanvasSize()
{
    return m_canvas_size_;
}

std::weak_ptr<Transform> Canvas::BoundsOrigin()
{
    return Camera::Main()->GameObject()->Transform();
}

void Canvas::AddRenderer(std::shared_ptr<Renderer2D> renderer)
{
    m_children_renderers_.emplace(renderer);
}

void Canvas::RemoveRenderer(std::shared_ptr<Renderer2D> renderer)
{
    m_children_renderers_.erase(renderer);
}
}