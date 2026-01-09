#include "pch.h"
#include "collider.h"
#include "compound_shape.h"
#include "game_object.h"

namespace engine
{
CompoundShape::CompoundShape(const std::shared_ptr<Transform> &target) :
    m_shape_(std::make_unique<btCompoundShape>()),
    m_transform_(target)
{ }

void CompoundShape::AddChild(const std::shared_ptr<Collider> &collider)
{
    RemoveChild(collider);

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

    const auto shape = collider->GetShape();
    if (shape != nullptr)
    {
        m_shape_->addChildShape(bt_transform, shape.get());
        m_colliders_.emplace_back(collider, shape);
    }
}

void CompoundShape::RemoveChild(const std::shared_ptr<Collider> &collider)
{
    auto colliders = m_colliders_;
    for (auto [col, shape] : colliders)
    {
        if (!col.expired() && collider != col.lock())
            continue;

        m_shape_->removeChildShape(shape.get());
    }

    std::erase_if(
        m_colliders_,
        [collider](auto &other) {
            return other.first.lock() == collider || other.first.expired();
        }
    );
}

void CompoundShape::UpdateShape()
{
    if (m_shape_ == nullptr)
        return;

    auto snapshot = m_colliders_;
    for (const auto &weak_collider : snapshot | std::views::keys)
    {
        auto collider = weak_collider.lock();
        if (collider == nullptr)
            continue;

        AddChild(collider);
    }
}

btCompoundShape *CompoundShape::GetShape() const
{
    return m_shape_.get();
}
}