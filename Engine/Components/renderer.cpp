#include "pch.h"

#include "renderer.h"

namespace engine
{
std::vector<std::weak_ptr<Renderer>> Renderer::m_renderers_;

void Renderer::SetVisible(const bool visible)
{
    if (m_is_visible_ == visible)
    {
        return;
    }

    m_is_visible_ = visible;

    if (m_is_visible_)
    {
        m_renderers_.emplace_back(shared_from_base<Renderer>());
    }
    else
    {
        const auto pos = std::ranges::find_if(m_renderers_, [&](const auto &r) {
            return r.lock() == shared_from_base<Renderer>();
        });
        if (pos == m_renderers_.end())
            return;
        m_renderers_.erase(pos);
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