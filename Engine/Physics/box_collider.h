#pragma once
#include "collider.h"

#include <BulletCollision/CollisionShapes/btBoxShape.h>

namespace engine
{
/// <summary>
/// 箱型のColliderです。
/// </summary>
class BoxCollider : public Collider
{
    std::shared_ptr<btBoxShape> m_box_shape_ = std::make_shared<btBoxShape>(btVector3{1.0F, 1.0F, 1.0F});
    Vector3 m_extents_ = {1.0F, 1.0F, 1.0F};

public:
    void OnInspectorGui() override;

    /// <summary>
    /// Extentsを 0 より大きい値に補正し、Boxの形状に反映します。
    /// </summary>
    void UpdateShape() override;
    /// <summary>
    /// BulletのBoxの形状を取得します。
    /// </summary>
    std::shared_ptr<btCollisionShape> GetShape() override;

    /// <summary>
    /// Boxの大きさを取得します。
    /// </summary>
    [[nodiscard]] Vector3 Extents() const;

    /// <summary>
    /// Boxの大きさを設定し、形状を更新します。
    /// </summary>
    /// <param name="extents">Boxの大きさ</param>
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