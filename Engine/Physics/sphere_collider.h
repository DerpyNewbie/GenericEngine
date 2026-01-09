#pragma once
#include "collider.h"

#include <BulletCollision/CollisionShapes/btSphereShape.h>

namespace engine
{
class SphereCollider final : public Collider
{
    std::shared_ptr<btSphereShape> m_shape_ = std::make_shared<btSphereShape>(1.0F);
    float m_radius_ = 1.0F;

public:
    void OnInspectorGui() override;

    void UpdateShape() override;
    std::shared_ptr<btCollisionShape> GetShape() override;

    [[nodiscard]] float Radius() const;
    void SetRadius(float radius);

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Collider>(this),
            CEREAL_NVP(m_radius_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::SphereCollider, 1)