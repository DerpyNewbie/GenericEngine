#pragma once
#include "collider.h"

#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>

namespace engine
{
class PlaneCollider : public Collider
{
    btStaticPlaneShape m_plane_ = {{0, 1, 0}, 0};

public:
    void UpdateShape() override;
    btCollisionShape *GetShape() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Collider>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::PlaneCollider, 1)