#include "pch.h"
#include "directional_light.h"

#include "game_object.h"

namespace engine
{
void DirectionalLight::OnAwake()
{
    m_lights_.emplace_back(shared_from_base<DirectionalLight>());
}
void DirectionalLight::OnUpdate()
{
    m_light_data_.direction = GameObject()->Transform()->Forward();
}
}