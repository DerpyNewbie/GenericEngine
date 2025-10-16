#include "pch.h"

#include "transform.h"
#include "game_object.h"
#include "gui.h"
#include "scene.h"

namespace engine
{

void Transform::OnInspectorGui()
{
    static int selected_tab = 0;

    if (ImGui::BeginTabBar("##TRANSFORM_TAB_BAR", ImGuiTabBarFlags_DrawSelectedOverline))
    {
        if (ImGui::BeginTabItem("Local", nullptr, selected_tab == 0 ? ImGuiTabItemFlags_SetSelected : 0))
        {
            TransformGui(true);
            ImGui::EndTabItem();
        }

        if (ImGui::IsItemClicked())
        {
            selected_tab = 0;
        }

        if (ImGui::BeginTabItem("Global", nullptr, selected_tab == 1 ? ImGuiTabItemFlags_SetSelected : 0))
        {
            TransformGui(false);
            ImGui::EndTabItem();
        }

        if (ImGui::IsItemClicked())
        {
            selected_tab = 1;
        }

        ImGui::EndTabBar();
    }
}

Matrix Transform::LocalMatrix() const
{
    return m_local_matrix_;
}

Matrix Transform::WorldMatrix() const
{
    return m_world_matrix_;
}

Matrix Transform::ParentMatrix() const
{
    const auto parent = Parent();
    return parent == nullptr ? Matrix::Identity : parent->WorldMatrix();
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
    return m_world_matrix_.Translation();
}

Quaternion Transform::Rotation() const
{
    auto world = m_world_matrix_;
    Vector3 scale, translation;
    Quaternion rotation;
    world.Decompose(scale, rotation, translation);
    return rotation;
}

Vector3 Transform::Scale() const
{
    auto world = m_world_matrix_;
    Vector3 scale, translation;
    Quaternion rotation;
    world.Decompose(scale, rotation, translation);
    return scale;
}

Vector3 Transform::LocalPosition() const
{
    return m_local_position_;
}

Quaternion Transform::LocalRotation() const
{
    return m_local_rotation_;
}

Vector3 Transform::LocalScale() const
{
    return m_local_scale_;
}

std::shared_ptr<Transform> Transform::Parent() const
{
    return m_parent_.lock();
}

std::shared_ptr<Transform> Transform::GetChild(const int i) const
{
    return m_children_.at(i);
}

int Transform::GetSiblingIndex() const
{
    const auto parent = Parent();
    if (parent == nullptr)
    {
        auto root_objects = GameObject()->Scene()->RootGameObjects();
        const auto itr = std::ranges::find(root_objects, GameObject());
        return static_cast<int>(std::distance(root_objects.begin(), itr));
    }

    auto children = parent->m_children_;
    auto guid = Guid();
    const auto itr = std::ranges::find_if(children, [&guid](auto &other) {
        return other->Guid() == guid;
    });
    return static_cast<int>(std::distance(children.begin(), itr));
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

void Transform::SetParent(const std::shared_ptr<Transform> &next_parent)
{
    const auto current_parent = m_parent_.lock();

    if (current_parent == next_parent)
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

    RecalculateMatrices();
    GameObject()->SetAsRootObject(m_parent_.expired());
}

void Transform::SetSiblingIndex(const int index)
{
    const auto parent = Parent();
    if (parent == nullptr)
    {
        const auto game_object = GameObject();
        const auto scene = GameObject()->Scene();
        auto &root_objects = scene->m_root_game_objects_;;
        std::erase(root_objects, game_object);
        if (index <= 0)
        {
            root_objects.insert(root_objects.begin(), game_object);
            return;
        }

        if (root_objects.size() < index)
        {
            root_objects.emplace_back(game_object);
            return;
        }

        root_objects.insert(root_objects.begin() + index, game_object);
        return;
    }

    const auto transform = shared_from_base<Transform>();
    auto &children = parent->m_children_;
    auto guid = Guid();
    std::erase_if(children, [&guid](auto &other) {
        return other->Guid() == guid;
    });

    if (index <= 0)
    {
        children.insert(children.begin(), transform);
        return;
    }

    if (children.size() < index)
    {
        children.emplace_back(transform);
        return;
    }

    children.insert(children.begin() + index, transform);
}

void Transform::SetAsFirstSibling()
{
    SetSiblingIndex(0);
}

void Transform::SetAsLastSibling()
{
    SetSiblingIndex(INT_MAX);
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
    m_local_position_ = local_position;
    RecalculateMatrices();
}

void Transform::SetLocalRotation(const Quaternion &local_rotation)
{
    m_local_rotation_ = local_rotation;
    RecalculateMatrices();
}

void Transform::SetLocalPositionAndRotation(const Vector3 &local_position, const Quaternion &local_rotation)
{
    m_local_position_ = local_position;
    m_local_rotation_ = local_rotation;
    RecalculateMatrices();
}

void Transform::SetLocalScale(const Vector3 &local_scale)
{
    m_local_scale_ = local_scale;
    RecalculateMatrices();
}

void Transform::SetLocalMatrix(const Matrix &matrix)
{
    auto mat = matrix;
    if (!mat.Decompose(m_local_scale_, m_local_rotation_, m_local_position_))
    {
        Logger::Warn<Transform>("Failed to decompose matrix. resetting to zero");
        m_local_position_ = Vector3::Zero;
        m_local_rotation_ = Quaternion::Identity;
        m_local_scale_ = Vector3::One;
    }

    RecalculateMatrices();
}

void Transform::TransformGui(const bool is_local)
{
    Vector3 position, rotation, scale;

    if (is_local)
    {
        position = LocalPosition();
        rotation = LocalRotation().ToEuler() * Mathf::kRad2Deg;
        scale = LocalScale();
    }
    else
    {
        position = Position();
        rotation = Rotation().ToEuler() * Mathf::kRad2Deg;
        scale = Scale();
    }

    if (Gui::PropertyField(is_local ? "Local Position" : "Position", position))
    {
        is_local ? SetLocalPosition(position) : SetPosition(position);
    }

    if (Gui::PropertyField(is_local ? "Local Rotation" : "Rotation", rotation))
    {
        const auto rot_quat = Quaternion::CreateFromYawPitchRoll(rotation * Mathf::kDeg2Rad);
        is_local ? SetLocalRotation(rot_quat) : SetRotation(rot_quat);
    }

    if (!is_local)
    {
        ImGui::BeginDisabled();
    }

    if (Gui::PropertyField(is_local ? "Local Scale" : "Scale", scale))
    {
        SetLocalScale(scale);
    }

    if (!is_local)
    {
        ImGui::EndDisabled();
    }

    if (ImGui::Button("Recalculate Matrices"))
    {
        RecalculateMatrices();
    }
}

Matrix Transform::TRS(const Vector3 translation, const Quaternion rotation, const Vector3 scale)
{
    return Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(translation);
}

void Transform::RecalculateMatrices()
{
    m_local_rotation_.Normalize();
    m_local_matrix_ = TRS(m_local_position_, m_local_rotation_, m_local_scale_);

    const auto parent = m_parent_.lock();
    if (parent == nullptr)
    {
        m_world_matrix_ = m_local_matrix_;
    }
    else
    {
        m_world_matrix_ = m_local_matrix_ * parent->WorldMatrix();
    }

    for (const auto child : m_children_)
    {
        child->RecalculateMatrices();
    }
}

void Transform::OnDestroy()
{
    SetParent(nullptr);

    for (const auto child : m_children_)
    {
        Destroy(child->GameObject());
    }
}
} // namespace engine

CEREAL_REGISTER_TYPE(engine::Transform)