#include "pch.h"

#include "controller.h"

#include "game_object.h"
#include "transform.h"
#include "Math/mathf.h"

namespace engine
{
void Controller::OnUpdate()
{
    Component::OnUpdate();

    constexpr float move_speed = 2.0f; // Adjust this value to control movement speed
    constexpr float rotate_speed = 2.0f * Mathf::kDeg2Rad; // Adjust this value to control rotation speed

    // Movement input
    Vector3 dir = {0, 0, 0};
    if (CheckHitKey(KEY_INPUT_W))
        dir.z -= 1.0f;
    if (CheckHitKey(KEY_INPUT_S))
        dir.z += 1.0f;
    if (CheckHitKey(KEY_INPUT_D))
        dir.x -= 1.0f;
    if (CheckHitKey(KEY_INPUT_A))
        dir.x += 1.0f;
    if (CheckHitKey(KEY_INPUT_SPACE))
        dir.y += 1.0f;
    if (CheckHitKey(KEY_INPUT_LCONTROL))
        dir.y -= 1.0f;

    // Normalize movement vector if not zero
    dir.Normalize();

    // Rotation input
    Vector3 delta_rot = {0, 0, 0};
    if (CheckHitKey(KEY_INPUT_UP))
        delta_rot.x += rotate_speed;
    if (CheckHitKey(KEY_INPUT_DOWN))
        delta_rot.x -= rotate_speed;
    if (CheckHitKey(KEY_INPUT_LEFT))
        delta_rot.y -= rotate_speed;
    if (CheckHitKey(KEY_INPUT_RIGHT))
        delta_rot.y += rotate_speed;

    // Get transform
    const auto transform = GameObject()->Transform();

    // Apply rotation around the object's position
    if (delta_rot.Length() > 0.001f)
    {
        m_rotation_ = m_rotation_ + delta_rot;
        transform->SetRotation(Quaternion::CreateFromYawPitchRoll(m_rotation_.y, m_rotation_.x, 0.0F));
    }

    // Apply movement in local space
    if (dir.Length() > 0.001f)
    {
        const Vector3 world_dir = Vector3::Transform(dir, transform->Rotation());
        const Vector3 new_pos = transform->Position() + (world_dir * move_speed);
        transform->SetPosition(new_pos);
    }
}
}

CEREAL_REGISTER_TYPE(engine::Controller)