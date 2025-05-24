#include "renderer.h"

#include "update_manager.h"
void engine::Renderer::OnAwake()
{
    Component::OnAwake();
    UpdateManager::SubscribeDrawCall(shared_from_base<Renderer>());
}