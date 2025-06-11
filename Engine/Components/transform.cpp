#include "transform.h"
#include "game_object.h"
#include "imgui.h"

namespace engine
{
Matrix4x4 Transform::ParentMatrix() const
{
    if (m_parent_.expired())
    {
        return MGetIdent();
    }

    const auto parent = m_parent_.lock();
    if (parent->m_parent_.expired())
    {
        return parent->m_matrix_;
    }

    // Use parent's LocalToWorld instead of direct recursion to ensure proper matrix chain
    return parent->LocalToWorld();
}

void Transform::OnInspectorGui()
{
    Component::OnInspectorGui();
    float local_pos[3];
    float local_rot_euler[3];
    float local_scale[3];

    EngineUtil::ToFloat3(local_pos, LocalPosition());
    EngineUtil::ToFloat3(local_rot_euler, LocalRotation().ToEulerDegrees());
    EngineUtil::ToFloat3(local_scale, LocalScale());

    ImGui::Text("Position: %f, %f, %f", local_pos[0], local_pos[1], local_pos[2]);
    if (ImGui::InputFloat3("Position", &local_pos[0]) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Log<Transform>("New Position: %f, %f, %f", local_pos[0], local_pos[1], local_pos[2]);
        SetLocalPosition({local_pos[0], local_pos[1], local_pos[2]});
    }
    if (ImGui::InputFloat3("Rotation", &local_rot_euler[0]) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Log<Transform>("New Rotation: %f, %f, %f", local_rot_euler[0], local_rot_euler[1], local_rot_euler[2]);
        SetLocalRotation(Quaternion::FromEulerDegrees({local_rot_euler[0], local_rot_euler[1], local_rot_euler[2]}));
    }
    if (ImGui::InputFloat3("Scale", &local_scale[0]) && ImGui::IsItemDeactivatedAfterEdit())
    {
        Logger::Log<Transform>("New Scale: %f, %f, %f", local_scale[0], local_scale[1], local_scale[2]);
        SetLocalScale({local_scale[0], local_scale[1], local_scale[2]});
    }
}
Matrix4x4 Transform::WorldToLocal() const
{
    return MInverse(LocalToWorld());
}

Matrix4x4 Transform::LocalToWorld() const
{
    return ParentMatrix() * m_matrix_;
}
Vector3 Transform::Position() const
{
    MATRIX local_to_world = LocalToWorld();
    return MGetTranslateElem(local_to_world);
}

Quaternion Transform::Rotation() const
{
    // Get rotation part of the world transform matrix
    Matrix4x4 rotation_matrix = MGetRotElem(LocalToWorld());

    // Extract quaternion using the robust method
    Quaternion rotation = Quaternion::FromMatrix(rotation_matrix);

    // Ensure it's normalized to prevent drift
    rotation.Normalize();
    return rotation;
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
    // Get rotation part of the local matrix
    Matrix4x4 rotation_matrix = MGetRotElem(m_matrix_);

    // Extract quaternion using the robust method
    Quaternion rotation = Quaternion::FromMatrix(rotation_matrix);

    // Ensure it's normalized to prevent drift
    rotation.Normalize();
    return rotation;
}

Vector3 Transform::LocalScale() const
{
    return MGetSize(m_matrix_ * MGetRotElem(MInverse(m_matrix_)));
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
    if (deep)
    {
        while (parent != nullptr)
        {
            if (parent == transform)
                return true;
            parent = parent->m_parent_.lock();
        }

        return false;
    }

    return parent == transform;
}
int Transform::ChildCount() const
{
    return static_cast<int>(m_children_.size());
}
void Transform::SetParent(const std::shared_ptr<Transform> &next_parent)
{
    // If parent doesn't change, ignore the call.
    auto parent = m_parent_.lock();
    if (parent == next_parent)
        return;

    // If we've had parent, Remove from parent's child
    if (parent != nullptr)
    {
        auto this_ptr = shared_from_base<Transform>();
        const auto pos =
            std::ranges::find_if(parent->m_children_,
                                 [&this_ptr](const std::weak_ptr<Transform> &other_ptr) {
                                     return other_ptr.lock() == this_ptr;
                                 });
        parent->m_children_.erase(pos);
    }

    m_parent_ = next_parent;

    // If parent is not null, Add to parent's child
    if ((parent = m_parent_.lock()))
    {
        parent->m_children_.push_back(shared_from_base<Transform>());
    }

    GameObject()->SetAsRootObject(m_parent_.expired());
}
void Transform::SetPosition(const Vector3 position)
{
    // If we have no parent, just set the local position directly
    if (m_parent_.expired())
    {
        SetLocalPosition(position);
        return;
    }

    // Transform world position to local space using parent's world-to-local matrix
    const Matrix4x4 parent_world_to_local = MInverse(ParentMatrix());
    const Vector3 local_position = position * parent_world_to_local;

    // Set the local position
    SetLocalPosition(local_position);
}

void Transform::SetRotation(const Quaternion &rotation)
{
    // If we have no parent, just set the local rotation directly
    if (m_parent_.expired())
    {
        SetLocalRotation(rotation);
        return;
    }

    // Get parent's world-to-local matrix to transform our rotation correctly
    const Matrix4x4 parent_world_to_local = MInverse(ParentMatrix());

    // Get parent's world rotation as a normalized quaternion
    const Quaternion parent_world_rotation = Quaternion::FromMatrix(MGetRotElem(ParentMatrix()));

    // Normalize before using the quaternion to prevent errors
    Quaternion normalized_rotation = rotation;
    normalized_rotation.Normalize();

    // Convert world rotation to local rotation by applying inverse of parent rotation
    const Quaternion local_rotation = parent_world_rotation.Inverse() * normalized_rotation;

    // Set the local rotation
    SetLocalRotation(local_rotation);
}
void Transform::SetLocalPosition(const Vector3 local_position)
{
    const Matrix4x4 trans = MGetTranslate(local_position);
    const Matrix4x4 rot = MGetRotElem(m_matrix_);
    const Matrix4x4 scale = MGetScale(LocalScale());
    m_matrix_ = scale * rot * trans;
}

void Transform::SetLocalRotation(const Quaternion &local_rotation)
{
    const Matrix4x4 trans = MGetTranslate(LocalPosition());
    const Matrix4x4 rot = local_rotation.ToMatrix();
    const Matrix4x4 scale = MGetScale(LocalScale());
    m_matrix_ = scale * rot * trans;
}

void Transform::SetLocalScale(const Vector3 local_scale)
{
    const Matrix4x4 trans = Matrix4x4::FromTranslate(LocalPosition());
    const Matrix4x4 rot = MGetRotElem(m_matrix_);
    const Matrix4x4 scale = MGetScale(local_scale);
    m_matrix_ = scale * rot * trans;
}
void Transform::SetLocalMatrix(const Matrix4x4 &matrix)
{
    m_matrix_ = matrix;
}

void Transform::RotateAround(const Vector3 &pivot_point, const Vector3 &axis, float angle_degrees)
{
    // Convert axis and angle to quaternion
    const float angle_radians = angle_degrees * (DX_PI_F / 180.0f);
    const Vector3 normalized_axis = VNorm(axis);
    const Quaternion rotation = Quaternion(normalized_axis.x * sinf(angle_radians * 0.5f),
                                           normalized_axis.y * sinf(angle_radians * 0.5f),
                                           normalized_axis.z * sinf(angle_radians * 0.5f),
                                           cosf(angle_radians * 0.5f));

    // Use the quaternion version
    RotateAround(pivot_point, rotation);
}

void Transform::RotateAround(const Vector3 &pivot_point, const Quaternion &rotation)
{
    // Get current position in world space
    Vector3 current_position = Position();

    // Step 1: Translate to make the pivot point the origin
    Vector3 position_relative_to_pivot = VSub(current_position, pivot_point);

    // Step 2: Rotate the position around the origin (now the pivot)
    Vector3 rotated_position = position_relative_to_pivot * rotation;

    // Step 3: Translate back
    Vector3 new_position = VAdd(rotated_position, pivot_point);

    // Step 4: Set the new position
    SetPosition(new_position);

    // Step 5: Apply the rotation to the object's current rotation
    Quaternion current_rotation = Rotation();
    SetRotation(current_rotation * rotation);
}
}

CEREAL_REGISTER_TYPE(engine::Transform)