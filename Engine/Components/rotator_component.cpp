#include "pch.h"

#include "rotator_component.h"

#include "engine_time.h"
#include "game_object.h"
#include "gui.h"

namespace engine
{
void RotatorComponent::OnUpdate()
{
    auto transform = GameObject()->Transform();
    auto local_rot = transform->LocalRotation();
    transform->SetLocalRotation(
        local_rot * Quaternion::CreateFromAxisAngle(m_rotating_axis_, Time::GetDeltaTime() * m_speed_));
}

void RotatorComponent::OnInspectorGui()
{
    Gui::PropertyField("Rotating Axis", m_rotating_axis_);
    Gui::PropertyField("Speed", m_speed_);
}
}