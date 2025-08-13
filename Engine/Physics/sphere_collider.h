#pragma once
#include "collider.h"

#include <BulletCollision/CollisionShapes/btSphereShape.h>

namespace engine
{
class SphereCollider final : public Collider
{
    btSphereShape m_shape_ = {1.0F};
    float m_radius_ = 1.0F;

public:
    void OnInspectorGui() override;

    void UpdateShape() override;
    btCollisionShape *GetShape() override;

    [[nodiscard]] float Radius() const;
    void SetRadius(float radius);

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Collider>(this), m_radius_);
    }
};
}