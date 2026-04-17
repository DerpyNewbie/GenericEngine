#include "pch.h"
#include "game_object.h"
#include "application.h"
#include "camera_component.h"
#include "gui.h"
#include "renderer.h"
#include "renderer_2d.h"
#include "canvas.h"

#include "Rendering/render_pipeline.h"

namespace
{
void GetSiblingIndices(const std::shared_ptr<engine::Transform> &a, std::vector<int> &sibling_indices)
{
    auto current = a;

    do
    {
        sibling_indices.emplace_back(current->GetSiblingIndex());

        current = current->Parent();
    } while (current->GameObject()->GetComponent<engine::Canvas>() == nullptr);
}
}

namespace engine
{
bool RendererComparator::operator()(const std::shared_ptr<Renderer2D> &a, const std::shared_ptr<Renderer2D> &b) const
{
    std::vector<int> a_sibling_indices = {};
    std::vector<int> b_sibling_indices = {};

    GetSiblingIndices(a->GameObject()->Transform(), a_sibling_indices);
    GetSiblingIndices(b->GameObject()->Transform(), b_sibling_indices);

    const int min_sibling_indices_size = std::min(a_sibling_indices.size(), b_sibling_indices.size());
    int itr = 0;
    while (itr < min_sibling_indices_size)
    {
        const int a_idx = a_sibling_indices[a_sibling_indices.size() - 1 - itr];//.sizeの差分の１を引いて置く
        const int b_idx = b_sibling_indices[b_sibling_indices.size() - 1 - itr];//.sizeの差分の１を引いて置く

        if (a_idx < b_idx)
            return true;
        if (b_idx < a_idx)
            return false;

        ++itr;
    }

    return false;
}

void Canvas::OnInspectorGui()
{
    Gui::PropertyField("Canvas Size", m_canvas_size_);
    Gui::PropertyField("Target Camera", m_target_camera_);
}

void Canvas::OnAwake()
{
    Renderer::OnAwake();
    m_canvas_size_ = Vector2{static_cast<float>(Application::WindowWidth()),
                             static_cast<float>(Application::WindowHeight())};
    if (m_target_camera_.Lock() == nullptr)
        m_target_camera_ = AssetPtr<CameraComponent>::FromManaged(CameraComponent::Main());
}

void Canvas::OnStart()
{
    Renderer::OnStart();
    const auto renderers = GameObject()->GetComponentsInChildren<Renderer2D>();

    for (auto renderer : renderers)
    {
        renderer->SetCanvas(shared_from_base<Canvas>());
        AddRenderer(renderer);
    }
}

void Canvas::Render()
{
    if (RenderPipeline::GetCurrentCamera() == m_target_camera_.CastedLock()->GetCamera())
        for (const auto it : m_child_renderers_)
            it->Render();
}

Vector2 Canvas::CanvasSize() const
{
    return m_canvas_size_;
}
Matrix Canvas::BoundsOrigin()
{
    return CameraComponent::Main()->GameObject()->Transform()->WorldMatrix();
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