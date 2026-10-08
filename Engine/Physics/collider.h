#pragma once
#include "rigidbody_component.h"
#include "Components/component.h"

namespace engine
{
/// <summary>
/// すべてのColliderの基底クラスです。Rigidbodyに登録されて衝突判定に使われます。
/// </summary>
class Collider : public Component
{
    friend class RigidbodyComponent;
    friend class CompoundShape;

    std::weak_ptr<RigidbodyComponent> m_rigidbody_;
    Vector3 m_offset_ = {0, 0, 0};
    bool m_is_trigger_ = false;
    bool m_is_registered_ = false;

    /// <summary>
    /// 現在の設定をBulletの形状に反映します。
    /// </summary>
    virtual void UpdateShape() = 0;
    /// <summary>
    /// Bulletの形状を取得します。
    /// </summary>
    virtual std::shared_ptr<btCollisionShape> GetShape() = 0;

    /// <summary>
    /// 自身または親のRigidbodyComponentに自身を登録します。見つからない場合は自身のGameObjectに追加します。
    /// </summary>
    void AddToRigidbody();
    /// <summary>
    /// 登録されているRigidbodyComponentから自身を取り除きます。
    /// </summary>
    void RemoveFromRigidbody();

protected:
    /// <summary>
    /// Rigidbodyに登録されている場合、一度取り除いて形状を更新し、登録し直します。
    /// </summary>
    void ApplyChanges();

public:
    void OnInspectorGui() override;
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    /// <summary>
    /// 登録されているRigidbodyComponentを取得します。
    /// </summary>
    [[nodiscard]] std::shared_ptr<RigidbodyComponent> Rigidbody() const;
    /// <summary>
    /// Triggerであるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsTrigger() const;
    /// <summary>
    /// GameObjectの位置からのOffsetを取得します。
    /// </summary>
    [[nodiscard]] Vector3 Offset() const;

    /// <summary>
    /// Triggerにするかどうかを設定し、Rigidbodyに反映します。
    /// </summary>
    /// <param name="trigger">Triggerにする場合 true</param>
    void SetTrigger(bool trigger);
    /// <summary>
    /// GameObjectの位置からのOffsetを設定し、Rigidbodyに反映します。
    /// </summary>
    /// <param name="offset">Offset</param>
    void SetOffset(Vector3 offset);

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_rigidbody_),
            CEREAL_NVP(m_offset_),
            CEREAL_NVP(m_is_trigger_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::Collider, 1)