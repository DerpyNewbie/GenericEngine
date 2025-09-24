#include "pch.h"
#include "directional_light.h"

#include "game_object.h"
#include "gui.h"

namespace engine
{
void DirectionalLight::OnInspectorGui()
{
    Light::OnInspectorGui();
    Gui::PropertyField("CoverSize", m_cover_size_);
}

void DirectionalLight::OnUpdate()
{
    Light::OnUpdate();
    auto transform = GameObject()->Transform();
    Vector3 forward = transform->Forward();
    m_light_data_.direction.x = forward.x;
    m_light_data_.direction.y = forward.y;
    m_light_data_.direction.z = forward.z;

    const Vector3 light_pos = -forward * m_cover_size_;

    transform->SetLocalPosition(light_pos);
}
}