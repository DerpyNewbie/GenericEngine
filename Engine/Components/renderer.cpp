#include "pch.h"

#include "renderer.h"

#include "update_manager.h"

void engine::Renderer::OnAwake()
{
    UpdateManager::SubscribeDrawCall(shared_from_base<Renderer>());
}
void engine::Renderer::OnDestroy()
{
    UpdateManager::UnsubscribeDrawCall(shared_from_base<Renderer>());
    Component::OnDestroy();
}