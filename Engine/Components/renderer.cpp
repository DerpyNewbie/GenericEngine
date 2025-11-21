#include "pch.h"

#include "renderer.h"

#include <algorithm>

#include "gui.h"
#include "Rendering/render_pipeline.h"

namespace engine
{
void Renderer::UpdateBuffer()
{}

void Renderer::DepthRender()
{}

void Renderer::SetVisible(const bool visible)
{
    if (m_is_visible_ == visible)
    {
        return;
    }

    m_is_visible_ = visible;

    if (m_is_visible_)
    {
        RenderPipeline::AddRenderer(shared_from_base<Renderer>());
    }
    else
    {
        RenderPipeline::RemoveRenderer(shared_from_base<Renderer>());
    }
}

void Renderer::OnInspectorGui()
{
    if (Gui::PropertyField("RenderQueue", m_render_queue_))
    {
        m_render_queue_ = std::clamp(m_render_queue_, 0, 10000);
    }
}

void Renderer::OnEnabled()
{
    SetVisible(true);
}

void Renderer::OnDisabled()
{
    SetVisible(false);
}

void Renderer::OnDestroy()
{
    SetVisible(false);
}
}

CEREAL_REGISTER_TYPE(engine::Renderer)