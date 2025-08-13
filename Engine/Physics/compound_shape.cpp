#include "pch.h"
#include "collider.h"
#include "compound_shape.h"
#include "game_object.h"

namespace engine
{
void CompoundShape::AddChildShape(const std::shared_ptr<Collider> &collider) const
{
    const auto collider_matrix = Matrix::CreateTranslation(collider->Offset()) *
                                 collider->GameObject()->Transform()->WorldMatrix();
    const auto this_matrix = m_transform_.lock()->WorldMatrix();
    auto relative_matrix = collider_matrix * this_matrix.Invert();

    Vector3 pos, sca;
    Quaternion rot;
    relative_matrix.Decompose(sca, rot, pos);

    btTransform bt_transform = btTransform::getIdentity();
    bt_transform.setOrigin({pos.x, pos.y, pos.z});
    bt_transform.setRotation({rot.x, rot.y, rot.z, rot.w});

    m_shape_->addChildShape(bt_transform, collider->GetShape());
}

void CompoundShape::RemoveChildShape(const std::shared_ptr<Collider> &collider) const
{
    m_shape_->removeChildShape(collider->GetShape());
}

CompoundShape::CompoundShape(const std::shared_ptr<Transform> &target) :
    m_shape_(std::make_unique<btCompoundShape>()),
    m_transform_(target)
{}

void CompoundShape::AddChild(const std::shared_ptr<Collider> &collider)
{
    m_colliders_.emplace_back(collider);
    AddChildShape(collider);
}

void CompoundShape::RemoveChild(const std::shared_ptr<Collider> &collider)
{
    std::erase_if(m_colliders_, [collider](auto &other) {
        return other.lock() == collider;
    });

    RemoveChildShape(collider);
}

void CompoundShape::UpdateShape() const
{
    for (auto &weak_collider : m_colliders_)
    {
        auto collider = weak_collider.lock();
        if (collider == nullptr)
            continue;

        RemoveChildShape(collider);
        AddChildShape(collider);
    }
}

btCompoundShape *CompoundShape::GetShape() const
{
    return m_shape_.get();
}

}