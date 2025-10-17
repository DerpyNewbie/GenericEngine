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

void Canvas::OnAwake()
{
    Renderer::OnAwake();
    if (m_target_camera_.Lock() == nullptr)
        m_target_camera_ = AssetPtr<CameraComponent>::FromManaged(CameraComponent::Main());
}
void Canvas::Render()
{
    if (CameraComponent::Current() == m_target_camera_.CastedLock())
        for (auto it : m_children_renderers_)
            it->Render();
}

Vector2 Canvas::CanvasSize()
{
    return m_canvas_size_;
}
std::shared_ptr<Transform> Canvas::BoundsOrigin()
{
    return CameraComponent::Main()->GameObject()->Transform();
}

void Canvas::AddRenderer(const std::shared_ptr<Renderer2D> &renderer)
{
    auto renderer_priority = renderer->GameObject()->Transform()->GetSiblingIndex();

    for (auto it = m_children_renderers_.begin(); it != m_children_renderers_.end(); ++it)
    {
        auto current_priority = (*it)->GameObject()->Transform()->GetSiblingIndex();

        if (renderer_priority < current_priority)
        {
            m_children_renderers_.insert(it, renderer);
            return;
        }
    }

    m_children_renderers_.emplace_back(renderer);
}

void Canvas::RemoveRenderer(const std::shared_ptr<Renderer2D> &renderer)
{
    std::erase_if(m_children_renderers_, [renderer](const std::shared_ptr<Renderer2D> &a) {
        return a == renderer;
    });
}
}

CEREAL_REGISTER_TYPE(engine::Canvas)