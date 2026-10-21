#include "pch.h"

#include "renderer.h"

#include <algorithm>

#include "gui.h"
#include "update_manager.h"

namespace engine
{
void Renderer::OnVisibilityChanged(bool visible)
{ }

void Renderer::SetVisible(const bool visible)
{
    if (m_is_visible_ == visible)
    {
        return;
    }

    m_is_visible_ = visible;

    if (m_is_visible_)
    {
        UpdateManager::SubscribeRender(shared_from_base<Renderer>());
    }
    else
    {
        UpdateManager::UnsubscribeRender(shared_from_base<Renderer>());
    }

    OnVisibilityChanged(m_is_visible_);
}

void Renderer::OnValidate()
{
    SetVisible(GameObject()->IsActiveInHierarchy());
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