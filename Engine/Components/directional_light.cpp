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
    Vector3 forward = GameObject()->Transform()->Forward();
    m_light_data_.direction.x = forward.x;
    m_light_data_.direction.y = forward.y;
    m_light_data_.direction.z = forward.z;
}
}