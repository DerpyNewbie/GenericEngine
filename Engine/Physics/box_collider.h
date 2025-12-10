#pragma once
#include "collider.h"

#include <BulletCollision/CollisionShapes/btBoxShape.h>

namespace engine
{
class BoxCollider : public Collider
{
    btBoxShape m_box_shape_ = {{1, 1, 1}};
    Vector3 m_extents_ = {1, 1, 1};

public:
    void OnInspectorGui() override;

    void UpdateShape() override;
    btCollisionShape *GetShape() override;

    [[nodiscard]] Vector3 Extents() const;

    void SetExtents(Vector3 extents);

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Collider>(this),
            CEREAL_NVP(m_extents_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::BoxCollider, 1)