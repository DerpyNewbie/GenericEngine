#include "pch.h"

#include "controller.h"

#include "engine_time.h"
#include "game_object.h"
#include "gui.h"
#include "transform.h"


namespace engine
{
void Controller::OnUpdate()
{
    Component::OnUpdate();

    // Movement input
    Vector3 dir = {0, 0, 0};
    float up_down = 0.0f;

    if (g_Input.IsKeyDown(Keyboard::W))
        dir.z -= 1.0f;
    if (g_Input.IsKeyDown(Keyboard::S))
        dir.z += 1.0f;
    if (g_Input.IsKeyDown(Keyboard::D))
        dir.x += 1.0f;
    if (g_Input.IsKeyDown(Keyboard::A))
        dir.x -= 1.0f;
    if (g_Input.IsKeyDown(Keyboard::Space))
        up_down += 1.0f;
    if (g_Input.IsKeyDown(Keyboard::LeftControl))
        up_down -= 1.0f;

    // Normalize movement vector if not zero
    dir.Normalize();

    m_last_movement_input_ = dir + Vector3::Up * up_down;
    m_last_movement_input_.Normalize();

    // Rotation input
    Vector2 delta_rot = {0, 0};
    if (g_Input.IsKeyDown(Keyboard::Up))
        delta_rot.x += rotate_speed;
    if (g_Input.IsKeyDown(Keyboard::Down))
        delta_rot.x -= rotate_speed;
    if (g_Input.IsKeyDown(Keyboard::Left))
        delta_rot.y += rotate_speed;
    if (g_Input.IsKeyDown(Keyboard::Right))
        delta_rot.y -= rotate_speed;

    m_last_rotation_input_ = delta_rot;

    // Get transform
    const auto transform = GameObject()->Transform();

    // Apply rotation around the object's position
    if (!Mathf::Approximately(delta_rot.Length(), 0.0F))
    {
        m_rotation_ = m_rotation_ + delta_rot * Time::GetDeltaTime();
        transform->SetRotation(Quaternion::CreateFromYawPitchRoll(m_rotation_.y, m_rotation_.x, 0.0F));
    }

    // Apply movement in local space
    if (!Mathf::Approximately(dir.Length(), 0.0F) || !Mathf::Approximately(up_down, 0.0F))
    {
        Vector3 world_dir = Vector3::Transform(dir, transform->Rotation()) + Vector3::Up * up_down;
        world_dir.Normalize();

        const Vector3 new_pos = transform->Position() + (world_dir * m_movement_speed_ * Time::GetDeltaTime());
        transform->SetPosition(new_pos);
    }
}

void Controller::OnInspectorGui()
{
    Gui::FloatField("Movement Speed", m_movement_speed_);
    Gui::FloatField("Rotation Speed", m_rotation_speed_);

    ImGui::Text("Last Movement Input: {%.2f, %.2f, %.2f}",
                m_last_movement_input_.x, m_last_movement_input_.y, m_last_movement_input_.z);
    ImGui::Text("Last Rotation Input: {%.2f, %.2f}",
                m_last_rotation_input_.x, m_last_rotation_input_.y);
}
}

CEREAL_REGISTER_TYPE(engine::Controller)