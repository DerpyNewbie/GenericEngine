#include "pch.h"
#include "game_object.h"
#include "application.h"
#include "camera_component.h"
#include "gui.h"
#include "renderer.h"
#include "renderer_2d.h"
#include "canvas.h"

namespace engine
{
bool RendererComparator::operator()(const std::shared_ptr<Renderer2D> &a, const std::shared_ptr<Renderer2D> &b) const
{
    return a->GameObject()->Transform()->GetSiblingIndex() < b->GameObject()->Transform()->GetSiblingIndex();
}

void Canvas::OnInspectorGui()
{
    Gui::PropertyField("Render Queue", m_render_queue_);
    Gui::PropertyField("Canvas Size", m_canvas_size_);
    Gui::PropertyField("Target Camera", m_target_camera_);
}

void Canvas::OnAwake()
{
    m_canvasses_.emplace(m_render_queue_, shared_from_base<Canvas>());
    m_canvas_size_ = Vector2{static_cast<float>(Application::WindowWidth()),
                             static_cast<float>(Application::WindowHeight())} * 0.5f;
    
    if (m_target_camera_.Lock() == nullptr)
        m_target_camera_ = AssetPtr<CameraComponent>::FromManaged(CameraComponent::Main());
}

void Canvas::OnStart()
{
    const auto renderers = GameObject()->GetComponentsInChildren<Renderer2D>();

    for (auto renderer : renderers)
    {
        renderer->SetCanvas(shared_from_base<Canvas>());
        AddRenderer(renderer);
    }
}

void Canvas::Render()
{
    if (CameraComponent::Current() == m_target_camera_.CastedLock())
        for (const auto it : m_child_renderers_)
            it->Render();
}

Vector2 Canvas::CanvasSize() const
{
    return m_canvas_size_;
}

void Canvas::AddRenderer(const std::shared_ptr<Renderer2D> &renderer)
{
    const auto renderer_priority = renderer->GameObject()->Transform()->GetSiblingIndex();

    // FIXME: this could break when child Renderer2D has changed its sibling position. has to update when it has marked dirty
    for (auto it = m_child_renderers_.begin(); it != m_child_renderers_.end(); ++it)
    {
        const auto current_priority = (*it)->GameObject()->Transform()->GetSiblingIndex();

        if (renderer_priority < current_priority)
        {
            m_child_renderers_.insert(it, renderer);
            return;
        }
    }

    m_child_renderers_.emplace(renderer);
}

void Canvas::RemoveRenderer(const std::shared_ptr<Renderer2D> &renderer)
{
    std::erase_if(m_child_renderers_, [renderer](const std::shared_ptr<Renderer2D> &a) {
        return a == renderer;
    });
}
}

CEREAL_REGISTER_TYPE(engine::Canvas)