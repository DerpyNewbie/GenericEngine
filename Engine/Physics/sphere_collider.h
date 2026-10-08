#pragma once
#include "collider.h"

#include <BulletCollision/CollisionShapes/btSphereShape.h>

namespace engine
{
/// <summary>
/// 球型のColliderです。
/// </summary>
class SphereCollider final : public Collider
{
    std::shared_ptr<btSphereShape> m_shape_ = std::make_shared<btSphereShape>(1.0F);
    float m_radius_ = 1.0F;

public:
    void OnInspectorGui() override;

    /// <summary>
    /// Radiusを 0 より大きい値に補正し、Sphereの形状に反映します。
    /// </summary>
    void UpdateShape() override;
    /// <summary>
    /// BulletのSphereの形状を取得します。
    /// </summary>
    std::shared_ptr<btCollisionShape> GetShape() override;

    /// <summary>
    /// Sphereの半径を取得します。
    /// </summary>
    [[nodiscard]] float Radius() const;
    /// <summary>
    /// Sphereの半径を設定し、形状を更新します。
    /// </summary>
    /// <param name="radius">半径</param>
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