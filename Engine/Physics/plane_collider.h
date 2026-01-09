#pragma once
#include "collider.h"

#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>

namespace engine
{
class PlaneCollider : public Collider
{
    std::shared_ptr<btStaticPlaneShape> m_plane_ = std::make_shared<btStaticPlaneShape>(btVector3{0.0F, 1.0F, 0.0F}, 0.0F);

public:
    void UpdateShape() override;
    std::shared_ptr<btCollisionShape> GetShape() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Collider>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::PlaneCollider, 1)