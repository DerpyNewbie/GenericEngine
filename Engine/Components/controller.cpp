#include "controller.h"
#include "game_object.h"
#include "transform.h"

namespace engine
{
void Controller::OnUpdate()
{
    Component::OnUpdate();

    const float move_speed = 0.1f; // Adjust this value to control movement speed
    const float rotate_speed = 2.0f; // Adjust this value to control rotation speed

    // Movement input
    VECTOR dir = {0, 0, 0};
    if (CheckHitKey(KEY_INPUT_W))
        dir.z += 1.0f;
    if (CheckHitKey(KEY_INPUT_S))
        dir.z -= 1.0f;
    if (CheckHitKey(KEY_INPUT_D))
        dir.x += 1.0f;
    if (CheckHitKey(KEY_INPUT_A))
        dir.x -= 1.0f;
    if (CheckHitKey(KEY_INPUT_SPACE))
        dir.y += 1.0f;
    if (CheckHitKey(KEY_INPUT_LCONTROL))
        dir.y -= 1.0f;

    // Normalize movement vector if not zero
    const float length = VSize(dir);
    if (length > 0.001f)
    {
        dir = VScale(dir, move_speed / length);
    }

    // Rotation input
    VECTOR delta_rot = {0, 0, 0};
    if (CheckHitKey(KEY_INPUT_UP))
        delta_rot.x += rotate_speed;
    if (CheckHitKey(KEY_INPUT_DOWN))
        delta_rot.x -= rotate_speed;
    if (CheckHitKey(KEY_INPUT_LEFT))
        delta_rot.y += rotate_speed;
    if (CheckHitKey(KEY_INPUT_RIGHT))
        delta_rot.y -= rotate_speed;

    // Get transform
    const auto transform = GameObject()->Transform();

    // Apply rotation around the object's position
    if (VSize(delta_rot) > 0.001f)
    {
        m_rotation_ = m_rotation_ + delta_rot;
        auto pitch = Quaternion::FromEulerDegrees({m_rotation_.x, 0, 0});
        auto yaw = Quaternion::FromEulerDegrees({0, m_rotation_.y, 0});
        transform->SetRotation(pitch * yaw);
    }

    // Apply movement in local space
    if (length > 0.001f)
    {
        VECTOR world_dir = dir * transform->Rotation();
        VECTOR new_pos = VAdd(transform->Position(), world_dir);
        transform->SetPosition(new_pos);
    }
}
}