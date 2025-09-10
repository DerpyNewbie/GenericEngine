#include "pch.h"

#include "renderer.h"

#include "Rendering/render_pipeline.h"

namespace engine
{
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