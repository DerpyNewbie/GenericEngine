#include "transform.h"
#include "game_object.h"
#include "imgui.h"

namespace engine
{

void Transform::OnInspectorGui()
{
    RenderLocalTransformGui();
    RenderGlobalTransformGui();
}
Matrix4x4 Transform::LocalMatrix() const
{
    return m_matrix_;
}
Matrix4x4 Transform::WorldMatrix() const
{
    return m_parent_.expired()
               ? m_matrix_
               : m_parent_.lock()->WorldMatrix() * m_matrix_;
}
Matrix4x4 Transform::WorldToLocal() const
{
    return WorldMatrix().Inverse();
}
Matrix4x4 Transform::LocalToWorld() const
{
    return WorldMatrix();
}
Vector3 Transform::Position() const
{
    MATRIX local_to_world = LocalToWorld();
    return MGetTranslateElem(local_to_world);
}
Quaternion Transform::Rotation() const
{
    return Quaternion::FromMatrix(MGetRotElem(LocalToWorld())).Normalized();
}
Vector3 Transform::Scale() const
{
    const Matrix4x4 local_to_world = LocalToWorld();
    return MGetSize(local_to_world * MGetRotElem(MInverse(local_to_world)));
}
Vector3 Transform::LocalPosition() const
{
    MATRIX v = m_matrix_;
    return MGetTranslateElem(v);
}
Quaternion Transform::LocalRotation() const
{
    return Quaternion::FromMatrix(MGetRotElem(m_matrix_)).Normalized();
}
Vector3 Transform::LocalScale() const
{
    return MGetSize(m_matrix_ * MInverse(MGetRotElem(m_matrix_)));
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

    const Vector3 local_position = position * m_parent_.lock()->WorldToLocal();
    SetLocalPosition(local_position);
}
void Transform::SetRotation(const Quaternion &rotation)
{
    if (m_parent_.expired())
    {
        SetLocalRotation(rotation);
        return;
    }

    const Quaternion parent_world_rot = Quaternion::FromMatrix(
        MGetRotElem(m_parent_.lock()->WorldToLocal()));
    SetLocalRotation(parent_world_rot.Inverse() * rotation.Normalized());
}

void Transform::SetPositionAndRotation(const Vector3 &position, const Quaternion &rotation)
{
    if (m_parent_.expired())
    {
        SetLocalPositionAndRotation(position, rotation);
        return;
    }

    const auto parent_world_to_local = m_parent_.lock()->WorldToLocal();
    const Vector3 local_position = position * parent_world_to_local;
    const Quaternion local_rotation = Quaternion::FromMatrix(MGetRotElem(parent_world_to_local)).Inverse()
                                      * rotation.Normalized();

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
void Transform::SetLocalMatrix(const Matrix4x4 &matrix)
{
    MATRIX m = matrix;
    const auto scale = MGetSize(m);
    const auto rotation = Quaternion::FromMatrix(MGetRotElem(m));
    const auto position = MGetTranslateElem(m);
    SetTRS(scale, rotation, position);
}
void Transform::SetTRS(const Vector3 &scale, const Quaternion &rotation, const Vector3 &position)
{
    m_matrix_ = static_cast<Matrix4x4>(MGetScale(scale)) * rotation.ToMatrix() * MGetTranslate(position);
}
void Transform::RotateAround(const Vector3 &pivot_point, const Vector3 &axis, const float angle_degrees)
{
    const float angle_radians = angle_degrees * (DX_PI_F / 180.0f);
    const Vector3 normalized_axis = VNorm(axis);
    const Quaternion rotation(
        normalized_axis.x * sinf(angle_radians * 0.5f),
        normalized_axis.y * sinf(angle_radians * 0.5f),
        normalized_axis.z * sinf(angle_radians * 0.5f),
        cosf(angle_radians * 0.5f)
        );

    RotateAround(pivot_point, rotation);
}
void Transform::RotateAround(const Vector3 &pivot_point, const Quaternion &rotation)
{
    const Vector3 position_relative_to_pivot = VSub(Position(), pivot_point);
    const Vector3 rotated_position = rotation * position_relative_to_pivot;
    const Vector3 new_position = VAdd(rotated_position, pivot_point);

    SetPosition(new_position);
    SetRotation(Rotation() * rotation);
}
void Transform::RenderLocalTransformGui()
{
    if (!ImGui::CollapsingHeader("Local"))
        return;

    ImGui::PushID("Local");

    float local_pos[3], local_rot_euler[3], local_scale[3];
    EngineUtil::ToFloat3(local_pos, LocalPosition());
    EngineUtil::ToFloat3(local_rot_euler, LocalRotation().ToEulerDegrees());
    EngineUtil::ToFloat3(local_scale, LocalScale());

    ImGui::Text("Position: %f, %f, %f", local_pos[0], local_pos[1], local_pos[2]);

    if (ImGui::InputFloat3("Position", local_pos) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Log<Transform>("New Position: %f, %f, %f", local_pos[0], local_pos[1], local_pos[2]);
        SetLocalPosition({local_pos[0], local_pos[1], local_pos[2]});
    }

    if (ImGui::InputFloat3("Rotation", local_rot_euler) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Log<Transform>("New Rotation: %f, %f, %f", local_rot_euler[0], local_rot_euler[1], local_rot_euler[2]);
        SetLocalRotation(Quaternion::FromEulerDegrees({local_rot_euler[0], local_rot_euler[1], local_rot_euler[2]}));
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
    EngineUtil::ToFloat3(global_rot_euler, Rotation().ToEulerDegrees());
    EngineUtil::ToFloat3(global_scale, Scale());

    ImGui::Text("Position: %f, %f, %f", global_pos[0], global_pos[1], global_pos[2]);

    if (ImGui::InputFloat3("Position", global_pos) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Log<Transform>("New Position: %f, %f, %f", global_pos[0], global_pos[1], global_pos[2]);
        SetPosition({global_pos[0], global_pos[1], global_pos[2]});
    }

    if (ImGui::InputFloat3("Rotation", global_rot_euler) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Log<Transform>("New Rotation: %f, %f, %f",
                               global_rot_euler[0], global_rot_euler[1], global_rot_euler[2]);
        SetRotation(Quaternion::FromEulerDegrees({global_rot_euler[0], global_rot_euler[1], global_rot_euler[2]}));
    }

    if (ImGui::InputFloat3("Scale", global_scale) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Error<Transform>("World scale is not implemented");
    }

    ImGui::PopID();
}

} // namespace engine

CEREAL_REGISTER_TYPE(engine::Transform)