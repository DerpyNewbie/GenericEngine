#include "transform.h"
#include "game_object.h"

namespace engine
{
MATRIX Transform::ParentMatrix() const
{
    if (m_parent_.expired() || m_parent_.lock()->m_parent_.expired())
    {
        return MGetIdent();
    }
    const auto parent = m_parent_.lock();
    return parent->m_parent_.lock()->ParentMatrix() * parent->m_matrix_;
}

MATRIX Transform::LocalToWorld() const
{
    return MInverse(ParentMatrix() * m_matrix_);
}

MATRIX Transform::WorldToLocal() const
{
    return ParentMatrix() * m_matrix_;
}

VECTOR Transform::Position() const
{
    MATRIX wtl = WorldToLocal();
    return MGetTranslateElem(wtl);
}

MATRIX Transform::Rotation() const
{
    const MATRIX wtl = WorldToLocal();
    return MGetRotElem(wtl);
}

VECTOR Transform::Scale() const
{
    const MATRIX wtl = WorldToLocal();
    return MGetSize(wtl * MGetRotElem(MInverse(wtl)));
}
VECTOR Transform::LocalPosition() const
{
    MATRIX v = m_matrix_;
    return MGetTranslateElem(v);
}
MATRIX Transform::LocalRotation() const
{
    return MGetRotElem(m_matrix_);
}

VECTOR Transform::LocalScale() const
{
    return MGetSize(m_matrix_ * MGetRotElem(MInverse(m_matrix_)));
}

std::shared_ptr<Transform> Transform::Parent() const
{
    return m_parent_.lock();
}
std::shared_ptr<Transform> Transform::GetChild(const int i) const
{
    return m_children_.at(i).lock();
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


void Transform::SetPosition(const VECTOR position)
{
    m_matrix_ = m_matrix_ * MGetTranslate(position - Position());
}

void Transform::SetRotation(const MATRIX &rotation)
{
    m_matrix_ = rotation * MInverse(Rotation()) * m_matrix_;
}

void Transform::SetLocalPosition(const VECTOR local_position)
{
    m_matrix_ = m_matrix_ * MGetTranslate(local_position - LocalPosition());
}

void Transform::SetLocalRotation(const MATRIX &local_rotation)
{
    m_matrix_ = local_rotation * MInverse(LocalRotation()) * m_matrix_;
}

void Transform::SetLocalScale(const VECTOR local_scale)
{
    m_matrix_ = MGetScale(LocalScale() - local_scale);
}
void Transform::SetLocalMatrix(const MATRIX &matrix)
{
    m_matrix_ = matrix;
}
}

CEREAL_REGISTER_TYPE(engine::Transform)