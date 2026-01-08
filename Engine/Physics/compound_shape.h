#pragma once

#include <BulletCollision/CollisionShapes/btCompoundShape.h>

namespace engine
{
class Transform;
class Collider;
class CompoundShape
{
    std::unique_ptr<btCompoundShape> m_shape_ = {};
    std::list<std::pair<std::weak_ptr<Collider>, std::shared_ptr<btCollisionShape>>> m_colliders_ = {};
    std::weak_ptr<Transform> m_transform_ = {};

public:
    CompoundShape() = default;
    explicit CompoundShape(const std::shared_ptr<Transform> &target);

    void AddChild(const std::shared_ptr<Collider> &collider);
    void RemoveChild(const std::shared_ptr<Collider> &collider);
    void UpdateShape();

    [[nodiscard]] btCompoundShape *GetShape() const;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(m_transform_)
        );

        if (!m_shape_)
            m_shape_ = std::make_unique<btCompoundShape>();
        UpdateShape();
    }
};
}

CEREAL_CLASS_VERSION(engine::CompoundShape, 2)