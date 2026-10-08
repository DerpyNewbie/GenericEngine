#pragma once

#include <BulletCollision/CollisionShapes/btCompoundShape.h>

namespace engine
{
class Transform;
class Collider;
/// <summary>
/// 複数のColliderの形状を1つにまとめた形状です。
/// </summary>
class CompoundShape : public Inspectable
{
    std::unique_ptr<btCompoundShape> m_shape_ = {};
    std::list<std::pair<std::weak_ptr<Collider>, std::shared_ptr<btCollisionShape>>> m_colliders_ = {};
    std::weak_ptr<Transform> m_transform_ = {};

public:
    CompoundShape() = default;
    /// <summary>
    /// 指定されたTransformを基準とする、空のCompoundShapeを作成します。
    /// </summary>
    /// <param name="target">基準となるTransform</param>
    explicit CompoundShape(const std::shared_ptr<Transform> &target);

    void OnInspectorGui() override;

    /// <summary>
    /// Colliderの形状を、基準のTransformからの相対的な位置と回転で追加します。既に追加されている場合は追加し直します。
    /// </summary>
    /// <param name="collider">追加するCollider</param>
    void AddChild(const std::shared_ptr<Collider> &collider);
    /// <summary>
    /// Colliderの形状を取り除きます。
    /// </summary>
    /// <param name="collider">取り除くCollider</param>
    void RemoveChild(const std::shared_ptr<Collider> &collider);
    /// <summary>
    /// 登録されているすべてのColliderを追加し直し、位置と回転を更新します。
    /// </summary>
    void UpdateShape();

    /// <summary>
    /// BulletのCompoundShapeを取得します。
    /// </summary>
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