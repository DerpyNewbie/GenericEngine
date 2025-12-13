#pragma once
#include "collider.h"

#include <BulletCollision/CollisionShapes/btCapsuleShape.h>

namespace engine
{
class CapsuleCollider : public Collider
{
    btCapsuleShape m_capsule_shape_ = {1, 0.5F};
    float m_radius_ = 1.0F;
    float m_height_ = 1.0F;

public:
    void OnInspectorGui() override;

    void UpdateShape() override;
    btCollisionShape *GetShape() override;

    void SetRadius(float radius);
    void SetHeight(float height);

    [[nodiscard]] float Radius() const;
    [[nodiscard]] float Height() const;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Collider>(this),
            CEREAL_NVP(m_radius_),
            CEREAL_NVP(m_height_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::CapsuleCollider, 1)