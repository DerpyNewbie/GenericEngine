#include "pch.h"

#include "cinema_camera_component.h"
#include "game_object.h"
#include "gui.h"

namespace engine
{
void CinemaCameraComponent::OnAwake()
{
    RecalculateOffset();
}

void CinemaCameraComponent::OnInspectorGui()
{
    if (Gui::PropertyField("Tracking Target", m_tracking_target_))
    {
        RecalculateOffset();
    }

    Gui::PropertyField("Apply Position", m_apply_position_);
    Gui::PropertyField("Apply Rotation", m_apply_rotation_);

    ImGui::Text("Tracking Offset: {%.2f, %.2f, %.2f}", m_tracking_offset_.x, m_tracking_offset_.y,
                m_tracking_offset_.z);

    auto look_at = GetLookAtRotation();
    auto look_at_euler = look_at.ToEuler() * Mathf::kRad2Deg;
    ImGui::Text("Look At: {%.2f, %.2f, %.2f}", look_at_euler.x, look_at_euler.y, look_at_euler.z);
    ImGui::Text("Position: {%.2f, %.2f, %.2f}", GetPosition().x, GetPosition().y, GetPosition().z);
    ImGui::Text("Target Position: {%.2f, %.2f, %.2f}", GetTargetPosition().x, GetTargetPosition().y,
                GetTargetPosition().z);
}

void CinemaCameraComponent::RecalculateOffset()
{
    const auto current_pos = GetPosition();
    const auto target_pos = GetTargetPosition();
    const auto forward = current_pos - target_pos;
    auto target_rot_inv = Quaternion::Identity;
    GetTargetRotation().Inverse(target_rot_inv);

    m_tracking_offset_ = Vector3::Transform(forward, target_rot_inv);
}

void CinemaCameraComponent::ApplyTransform()
{
    const auto transform = GameObject()->Transform();
    const auto next_pos = GetTargetPosition() + Vector3::Transform(m_tracking_offset_, GetTargetRotation());

    if (m_apply_position_)
    {
        transform->SetPosition(next_pos);
    }

    if (m_apply_rotation_)
    {
        transform->SetRotation(GetLookAtRotation());
    }
}

Vector3 CinemaCameraComponent::GetTargetPosition()
{
    const auto lock = m_tracking_target_.CastedLock();
    return lock != nullptr ? lock->Position() : Vector3::Zero;
}

Quaternion CinemaCameraComponent::GetTargetRotation()
{
    const auto lock = m_tracking_target_.CastedLock();
    return lock != nullptr ? lock->Rotation() : Quaternion::Identity;
}

Vector3 CinemaCameraComponent::GetPosition() const
{
    return GameObject()->Transform()->Position();
}

Quaternion CinemaCameraComponent::GetRotation() const
{
    return GameObject()->Transform()->Rotation();
}

Quaternion CinemaCameraComponent::GetLookAtRotation()
{
    auto look_at = GetLookAtMatrix();
    Vector3 pos, sca;
    Quaternion rot;
    look_at.Decompose(sca, rot, pos);
    return rot;
}
Matrix CinemaCameraComponent::GetLookAtMatrix()
{
    const auto position = GetPosition();
    const auto target_pos = GetTargetPosition();

    if (position == Vector3::Zero || position == target_pos)
        return Matrix::Identity;

    const Matrix view = Matrix::CreateLookAt(position, target_pos, Vector3::Up);
    return view.Invert();
}
}

CEREAL_REGISTER_TYPE(engine::CinemaCameraComponent)