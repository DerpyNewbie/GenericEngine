#pragma once

#include <BulletCollision/CollisionShapes/btCompoundShape.h>

namespace engine
{
class Transform;
class Collider;
class CompoundShape
{
    std::unique_ptr<btCompoundShape> m_shape_;
    std::list<std::weak_ptr<Collider>> m_colliders_;
    std::weak_ptr<Transform> m_transform_;

    void AddChildShape(const std::shared_ptr<Collider> &collider) const;
    void RemoveChildShape(const std::shared_ptr<Collider> &collider) const;

public:
    CompoundShape() = default;
    explicit CompoundShape(const std::shared_ptr<Transform> &target);

    void AddChild(const std::shared_ptr<Collider> &collider);
    void RemoveChild(const std::shared_ptr<Collider> &collider);
    void UpdateShape() const;

    [[nodiscard]] btCompoundShape *GetShape() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(m_colliders_), CEREAL_NVP(m_transform_));

        UpdateShape();
    }
};
}