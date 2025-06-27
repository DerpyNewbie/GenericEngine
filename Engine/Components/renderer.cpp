#include "pch.h"

#include "renderer.h"

#include "update_manager.h"

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
        UpdateManager::SubscribeDrawCall(shared_from_base<Renderer>());
    }
    else
    {
        UpdateManager::UnsubscribeDrawCall(shared_from_base<Renderer>());
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