#include "pch.h"
#include "directional_light.h"

#include "game_object.h"

namespace engine
{
void DirectionalLight::OnUpdate()
{
    Light::OnUpdate();
    auto transform = GameObject()->Transform();
    Vector3 forward = transform->Forward();
    m_light_data_.direction.x = forward.x;
    m_light_data_.direction.y = forward.y;
    m_light_data_.direction.z = forward.z;

    const Vector3 light_pos = -forward * 10.0f;

    transform->SetLocalPosition(light_pos);
}
}