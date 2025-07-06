#include "pch.h"

#include "transform.h"
#include "game_object.h"

namespace engine
{

void Transform::OnInspectorGui()
{
    RenderLocalTransformGui();
    RenderGlobalTransformGui();
}
Matrix Transform::LocalMatrix() const
{
    return m_matrix_;
}
Matrix Transform::WorldMatrix() const
{
    return m_parent_.expired()
               ? m_matrix_
               : m_parent_.lock()->WorldMatrix() * m_matrix_;
}
Matrix Transform::WorldToLocal() const
{
    return WorldMatrix().Invert();
}
Matrix Transform::LocalToWorld() const
{
    return WorldMatrix();
}
Vector3 Transform::Position() const
{
    Vector3 pos, sca;
    Quaternion rot;
    LocalToWorld().Decompose(sca, rot, pos);
    return pos;
}
Quaternion Transform::Rotation() const
{
    Vector3 pos, sca;
    Quaternion rot;
    LocalToWorld().Decompose(sca, rot, pos);
    return rot;
}
Vector3 Transform::Scale() const
{
    Vector3 pos, sca;
    Quaternion rot;
    LocalToWorld().Decompose(sca, rot, pos);
    return sca;
}
Vector3 Transform::LocalPosition() const
{
    Vector3 pos, sca;
    Quaternion rot;
    LocalMatrix().Decompose(sca, rot, pos);
    return pos;
}
Quaternion Transform::LocalRotation() const
{
    Vector3 pos, sca;
    Quaternion rot;
    LocalMatrix().Decompose(sca, rot, pos);
    return rot;
}
Vector3 Transform::LocalScale() const
{
    Vector3 pos, sca;
    Quaternion rot;
    LocalMatrix().Decompose(sca, rot, pos);
    return sca;
}
std::shared_ptr<Transform> Transform::Parent() const
{
    return m_parent_.lock();
}
std::shared_ptr<Transform> Transform::GetChild(const int i) const
{
    return m_children_.at(i);
}
bool Transform::IsChildOf(const std::shared_ptr<Transform> &transform, const bool deep) const
{
    auto parent = m_parent_.lock();
    if (!deep)
        return parent == transform;

    while (parent)
    {
        if (parent == transform)
            return true;
        parent = parent->m_parent_.lock();
    }
    return false;
}
int Transform::ChildCount() const
{
    return static_cast<int>(m_children_.size());
}
void Transform::SetParent(const std::weak_ptr<Transform> &next_parent)
{
    const auto current_parent = m_parent_.lock();
    const auto new_parent = next_parent.lock();

    if (current_parent == new_parent)
        return;

    if (current_parent)
    {
        auto this_ptr = shared_from_base<Transform>();
        const auto it = std::ranges::find_if(current_parent->m_children_,
                                             [&this_ptr](const auto &ptr) {
                                                 return ptr == this_ptr;
                                             });
        current_parent->m_children_.erase(it);
    }

    m_parent_ = next_parent;

    if (const auto parent = m_parent_.lock())
    {
        parent->m_children_.emplace_back(shared_from_base<Transform>());
    }

    GameObject()->SetAsRootObject(m_parent_.expired());
}
void Transform::SetPosition(const Vector3 &position)
{
    if (m_parent_.expired())
    {
        SetLocalPosition(position);
        return;
    }

    const Vector3 local_position;
    position.Transform(local_position, m_parent_.lock()->WorldToLocal());
    SetLocalPosition(local_position);
}
void Transform::SetRotation(const Quaternion &rotation)
{
    if (m_parent_.expired())
    {
        SetLocalRotation(rotation);
        return;
    }
    const Quaternion parent_world_rot = Quaternion::CreateFromRotationMatrix(m_parent_.lock()->WorldToLocal());
    Quaternion inversed_world_rot;
    parent_world_rot.Inverse(inversed_world_rot);
    SetLocalRotation(inversed_world_rot * rotation);
}

void Transform::SetPositionAndRotation(const Vector3 &position, const Quaternion &rotation)
{
    if (m_parent_.expired())
    {
        SetLocalPositionAndRotation(position, rotation);
        return;
    }

    const auto parent_world_to_local = m_parent_.lock()->WorldToLocal();
    const Vector3 local_position;
    position.Transform(local_position, parent_world_to_local);

    const Quaternion parent_world_to_local_rot = Quaternion::CreateFromRotationMatrix(parent_world_to_local);
    Quaternion inversed_parent_world_to_local_rot;
    parent_world_to_local_rot.Inverse(inversed_parent_world_to_local_rot);

    const Quaternion local_rotation = inversed_parent_world_to_local_rot * rotation;
    SetLocalPositionAndRotation(local_position, local_rotation);
}

void Transform::SetLocalPosition(const Vector3 &local_position)
{
    SetTRS(LocalScale(), LocalRotation(), local_position);
}

void Transform::SetLocalRotation(const Quaternion &local_rotation)
{
    SetTRS(LocalScale(), local_rotation, LocalPosition());
}

void Transform::SetLocalPositionAndRotation(const Vector3 &local_position, const Quaternion &local_rotation)
{
    SetTRS(LocalScale(), local_rotation, local_position);
}
void Transform::SetLocalScale(const Vector3 &local_scale)
{
    SetTRS(local_scale, LocalRotation(), LocalPosition());
}
void Transform::SetLocalMatrix(const Matrix &matrix)
{
    m_matrix_ = matrix;
}
void Transform::SetTRS(const Vector3 &scale, const Quaternion &rotation, const Vector3 &position)
{
    m_matrix_ = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) *
                Matrix::CreateTranslation(position);
}
void Transform::RenderLocalTransformGui()
{
    if (!ImGui::CollapsingHeader("Local", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    ImGui::PushID("Local");

    float local_pos[3], local_rot_euler[3], local_scale[3];
    EngineUtil::ToFloat3(local_pos, LocalPosition());
    EngineUtil::ToFloat3(local_rot_euler, LocalRotation().ToEuler() * Mathf::kRad2Deg);
    EngineUtil::ToFloat3(local_scale, LocalScale());

    if (ImGui::InputFloat3("Position", local_pos) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Log<Transform>("New Position: %f, %f, %f", local_pos[0], local_pos[1], local_pos[2]);
        SetLocalPosition({local_pos[0], local_pos[1], local_pos[2]});
    }

    if (ImGui::InputFloat3("Rotation", local_rot_euler) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Log<Transform>("New Rotation: %f, %f, %f", local_rot_euler[0], local_rot_euler[1], local_rot_euler[2]);
        SetLocalRotation(
            Quaternion::CreateFromYawPitchRoll(
                local_rot_euler[1] * Mathf::kDeg2Rad,
                local_rot_euler[0] * Mathf::kDeg2Rad,
                local_rot_euler[2] * Mathf::kDeg2Rad));
    }

    if (ImGui::InputFloat3("Scale", local_scale) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Log<Transform>("New Scale: %f, %f, %f", local_scale[0], local_scale[1], local_scale[2]);
        SetLocalScale({local_scale[0], local_scale[1], local_scale[2]});
    }

    ImGui::PopID();
}
void Transform::RenderGlobalTransformGui()
{
    if (!ImGui::CollapsingHeader("Global"))
        return;

    ImGui::PushID("Global");

    float global_pos[3], global_rot_euler[3], global_scale[3];
    EngineUtil::ToFloat3(global_pos, Position());
    EngineUtil::ToFloat3(global_rot_euler, Rotation().ToEuler() * Mathf::kRad2Deg);
    EngineUtil::ToFloat3(global_scale, Scale());

    if (ImGui::InputFloat3("Position", global_pos) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Log<Transform>("New Position: %f, %f, %f", global_pos[0], global_pos[1], global_pos[2]);
        SetPosition({global_pos[0], global_pos[1], global_pos[2]});
    }

    if (ImGui::InputFloat3("Rotation", global_rot_euler) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Log<Transform>("New Rotation: %f, %f, %f",
                               global_rot_euler[0], global_rot_euler[1], global_rot_euler[2]);
        SetRotation(Quaternion::CreateFromYawPitchRoll(
            global_rot_euler[1] * Mathf::kDeg2Rad,
            global_rot_euler[0] * Mathf::kDeg2Rad,
            global_rot_euler[2] * Mathf::kDeg2Rad));
    }

    if (ImGui::InputFloat3("Scale", global_scale) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Error<Transform>("World scale is not implemented");
    }

    ImGui::PopID();
}

void Transform::OnDestroy()
{
    for (const auto child : m_children_)
    {
        DestroyImmediate(child->GameObject());
    }
}
} // namespace engine

CEREAL_REGISTER_TYPE(engine::Transform)