#pragma once
#include "collider.h"

#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>

namespace engine
{
/// <summary>
/// 無限に広がる平面のColliderです。
/// </summary>
class PlaneCollider : public Collider
{
    std::shared_ptr<btStaticPlaneShape> m_plane_ = std::make_shared<btStaticPlaneShape>(btVector3{0.0F, 1.0F, 0.0F}, 0.0F);

public:
    /// <summary>
    /// 何もしません。Planeは形状を更新する必要がありません。
    /// </summary>
    void UpdateShape() override;
    /// <summary>
    /// BulletのPlaneの形状を取得します。
    /// </summary>
    std::shared_ptr<btCollisionShape> GetShape() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Collider>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::PlaneCollider, 1)