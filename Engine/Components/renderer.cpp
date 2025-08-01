#include "pch.h"

#include "renderer.h"

namespace engine
{
std::vector<std::weak_ptr<Renderer>> Renderer::renderers;

void Renderer::SetVisible(const bool visible)
{
    if (m_is_visible_ == visible)
    {
        return;
    }

    m_is_visible_ = visible;

    if (m_is_visible_)
    {
        renderers.emplace_back(shared_from_base<Renderer>());
    }
    else
    {
        const auto pos = std::ranges::find_if(renderers, [&](const auto &r) {
            return r.lock() == shared_from_base<Renderer>();
        });
        if (pos == renderers.end())
            return;
        renderers.erase(pos);
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