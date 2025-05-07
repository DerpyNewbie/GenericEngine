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
    return MMult(parent->m_parent_.lock()->ParentMatrix(), parent->m_matrix_);
}

MATRIX Transform::Compose(const VECTOR &scale, const MATRIX &rotation, const VECTOR &translation)
{
    return MMult(MGetScale(scale), MMult(MGetRotElem(rotation), MGetTranslate(translation)));
}

MATRIX Transform::LocalToWorld() const
{
    return MInverse(MMult(ParentMatrix(), m_matrix_));
}

MATRIX Transform::WorldToLocal() const
{
    return MMult(ParentMatrix(), m_matrix_);
}

VECTOR Transform::Position() const
{
    MATRIX ltw = LocalToWorld();
    return MGetTranslateElem(ltw);
}

MATRIX Transform::Rotation() const
{
    const MATRIX ltw = LocalToWorld();
    return MGetRotElem(ltw);
}

VECTOR Transform::Scale() const
{
    const MATRIX ltw = LocalToWorld();
    return MGetSize(MMult(ltw, MGetRotElem(MInverse(ltw))));
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
    return MGetSize(MMult(m_matrix_, MGetRotElem(MInverse(m_matrix_))));
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
    m_matrix_ = Compose(LocalScale(), LocalRotation(), VTransform(position, WorldToLocal()));
}

void Transform::SetRotation(const MATRIX &rotation)
{
    m_matrix_ = Compose(LocalScale(), MMult(rotation, MGetRotElem(WorldToLocal())), LocalPosition());
}

void Transform::SetLocalPosition(const VECTOR local_position)
{
    m_matrix_ = Compose(LocalScale(), LocalRotation(), local_position);
}

void Transform::SetLocalRotation(const MATRIX &rotation)
{
    m_matrix_ = Compose(LocalScale(), MGetRotElem(rotation), LocalPosition());
}

void Transform::SetLocalScale(const VECTOR local_scale)
{
    m_matrix_ = Compose(local_scale, LocalRotation(), LocalPosition());
}
}

CEREAL_REGISTER_TYPE(engine::Transform)

CEREAL_REGISTER_POLYMORPHIC_RELATION(engine::Component, engine::Transform)