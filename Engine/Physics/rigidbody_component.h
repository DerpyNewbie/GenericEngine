#pragma once
#include "compound_shape.h"
#include "Components/component.h"
#include "Components/transform.h"

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet/LinearMath/btMotionState.h>

namespace engine
{
class Collider;
/// <summary>
/// Rigidbodyの移動 / 回転を固定する軸です。
/// </summary>
enum kLockAxis
{
    kNone = 0,
    kAxisX = 0x1,
    kAxisY = 0x2,
    kAxisZ = 0x4,
    kAxisAll = kAxisX | kAxisY | kAxisZ
};

/// <summary>
/// Rigidbodyに力を加える方法です。
/// </summary>
enum class kForceMode : unsigned char
{
    kForce,
    kImpulse
};

/// <summary>
/// GameObjectを物理シミュレーションの対象にするComponentです。
/// </summary>
class RigidbodyComponent : public Component
{
    friend class Physics;
    friend class Collider;

    std::unique_ptr<btRigidBody> m_bt_rigidbody_ = nullptr;
    std::unique_ptr<btGhostObject> m_bt_ghost_object_ = nullptr;
    std::unique_ptr<btMotionState> m_bt_motion_state_ = nullptr;
    std::unique_ptr<CompoundShape> m_rigidbody_shape_ = nullptr;
    std::unique_ptr<CompoundShape> m_ghost_shape_ = nullptr;

    std::list<std::weak_ptr<class GameObject>> m_last_ghost_overlapping_objects_;

    std::weak_ptr<Transform> m_transform_ = {};

    bool m_should_write_ = true;
    bool m_is_registered_ = false;

    Matrix m_last_world_matrix_ = {};

    Vector3 m_velocity_ = {};
    Vector3 m_angular_velocity_ = {};
    Vector3 m_center_of_mass_ = {};

    float m_mass_ = 1;
    float m_linear_damping_ = 0.0F;
    float m_angular_damping_ = 0.0F;
    float m_friction_ = 0.5F;
    float m_rolling_friction_ = 0.0F;
    float m_spinning_friction_ = 0.0F;
    float m_bounciness_ = 0.0F;

    bool m_is_kinematic_ = true;
    bool m_is_static_ = true;
    bool m_use_gravity_ = true;
    kLockAxis m_lock_axis_ = kNone;

    /// <summary>
    /// まだ作成されていない場合、MotionState、CompoundShape、BulletのRigidbody、Trigger用のGhostObjectを作成します。
    /// </summary>
    void ConstructRigidbody();
    /// <summary>
    /// 必要なものを作成し、Physicsに自身を登録します。既に登録されている場合は登録し直します。
    /// </summary>
    void RegisterToPhysics();
    /// <summary>
    /// BulletのRigidbodyから、位置、回転、速度、角速度を読み取ります。
    /// </summary>
    void ReadRigidbody();
    /// <summary>
    /// 現在の設定(Transform、速度、摩擦、質量、Kinematic / Static、軸の固定、重力など)をBulletのRigidbodyに書き込みます。
    /// </summary>
    void WriteRigidbody();
    /// <summary>
    /// BulletのRigidbodyの位置と回転をTransformに反映します。
    /// </summary>
    void ReadTransform();
    /// <summary>
    /// Transformの位置と回転をBulletのRigidbodyに書き込みます。
    /// </summary>
    void WriteTransform();
    /// <summary>
    /// Physicsから自身を取り除きます。
    /// </summary>
    void UnregisterFromPhysics();
    /// <summary>
    /// Rigidbody用とTrigger用のCompoundShapeを更新します。
    /// </summary>
    void UpdateCompoundShape() const;
    /// <summary>
    /// Physicsに登録されている場合、一度取り除いて設定を書き込み、登録し直します。
    /// </summary>
    void UpdatePhysics();

    /// <summary>
    /// シミュレーションの前に呼ばれます。Transformが外部から変更されていた場合や、設定が変更されていた場合に、Bulletへ書き込みます。
    /// </summary>
    void OnPrePhysicsUpdate();
    /// <summary>
    /// シミュレーションの後に呼ばれます。Bulletの結果を読み取ります。
    /// </summary>
    void OnPostPhysicsUpdate();

    /// <summary>
    /// GhostObjectと重なっているRigidbodyを調べ、Triggerの重なりとしてPhysicsに記録します。
    /// </summary>
    void CollectOverlaps() const;

    /// <summary>
    /// ColliderをCompoundShapeに追加します。Triggerの場合はTrigger用のShapeに追加されます。
    /// </summary>
    /// <param name="collider">追加するCollider</param>
    void AddCollider(const std::shared_ptr<Collider> &collider) const;
    /// <summary>
    /// ColliderをCompoundShapeから取り除きます。
    /// </summary>
    /// <param name="collider">取り除くCollider</param>
    void RemoveCollider(const std::shared_ptr<Collider> &collider) const;

public:
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    void OnInspectorGui() override;

    /// <summary>
    /// 対象のTransformを取得します。
    /// </summary>
    [[nodiscard]] std::shared_ptr<Transform> Transform();
    /// <summary>
    /// 速度を取得します。
    /// </summary>
    [[nodiscard]] Vector3 Velocity() const;
    /// <summary>
    /// 角速度を取得します。
    /// </summary>
    [[nodiscard]] Vector3 AngularVelocity() const;
    /// <summary>
    /// 重心の位置(Local空間)を取得します。
    /// </summary>
    [[nodiscard]] Vector3 CenterOfMass() const;
    /// <summary>
    /// 質量を取得します。
    /// </summary>
    [[nodiscard]] float Mass() const;
    /// <summary>
    /// 摩擦を取得します。
    /// </summary>
    [[nodiscard]] float Friction() const;
    /// <summary>
    /// 転がり摩擦を取得します。
    /// </summary>
    [[nodiscard]] float RollingFriction() const;
    /// <summary>
    /// 回転摩擦を取得します。
    /// </summary>
    [[nodiscard]] float SpinningFriction() const;
    /// <summary>
    /// 反発係数を取得します。
    /// </summary>
    [[nodiscard]] float Bounciness() const;
    /// <summary>
    /// 速度の減衰を取得します。
    /// </summary>
    [[nodiscard]] float LinearDamping() const;
    /// <summary>
    /// 角速度の減衰を取得します。
    /// </summary>
    [[nodiscard]] float AngularDamping() const;
    /// <summary>
    /// スリープ状態(または未作成)であるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsSleeping() const;
    /// <summary>
    /// Kinematicであるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsKinematic() const;
    /// <summary>
    /// Staticであるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsStatic() const;
    /// <summary>
    /// 重力の影響を受けるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool UseGravity() const;
    /// <summary>
    /// 回転を固定している軸を取得します。
    /// </summary>
    [[nodiscard]] kLockAxis LockAxis() const;
    /// <summary>
    /// KinematicまたはStaticであるかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsKinematicOrStatic() const;
    /// <summary>
    /// KinematicでもStaticでもないかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool IsDynamic() const;

    /// <summary>
    /// TransformのWorld空間での位置を設定します。
    /// </summary>
    /// <param name="position">World空間での位置</param>
    void SetPosition(const Vector3 &position);
    /// <summary>
    /// TransformのWorld空間での回転を設定します。
    /// </summary>
    /// <param name="rotation">World空間での回転</param>
    void SetRotation(const Quaternion &rotation);
    /// <summary>
    /// 速度を設定します。次のシミュレーションの前に反映されます。
    /// </summary>
    /// <param name="velocity">速度</param>
    void SetVelocity(const Vector3 &velocity);
    /// <summary>
    /// 角速度を設定します。次のシミュレーションの前に反映されます。
    /// </summary>
    /// <param name="angular_velocity">角速度</param>
    void SetAngularVelocity(const Vector3 &angular_velocity);
    /// <summary>
    /// 重心の位置(Local空間)を設定します。次のシミュレーションの前に反映されます。
    /// </summary>
    /// <param name="center_of_mass">重心の位置</param>
    void SetCenterOfMass(const Vector3 &center_of_mass);
    /// <summary>
    /// 質量を設定します。次のシミュレーションの前に反映されます。
    /// </summary>
    /// <param name="mass">質量</param>
    void SetMass(float mass);
    /// <summary>
    /// 摩擦を設定します。次のシミュレーションの前に反映されます。
    /// </summary>
    /// <param name="friction">摩擦</param>
    void SetFriction(float friction);
    /// <summary>
    /// 転がり摩擦を設定します。次のシミュレーションの前に反映されます。
    /// </summary>
    /// <param name="rolling_friction">転がり摩擦</param>
    void SetRollingFriction(float rolling_friction);
    /// <summary>
    /// 回転摩擦を設定します。次のシミュレーションの前に反映されます。
    /// </summary>
    /// <param name="spinning_friction">回転摩擦</param>
    void SetSpinningFriction(float spinning_friction);
    /// <summary>
    /// 反発係数を設定します。次のシミュレーションの前に反映されます。
    /// </summary>
    /// <param name="bounciness">反発係数</param>
    void SetBounciness(float bounciness);
    /// <summary>
    /// 速度の減衰を設定します。次のシミュレーションの前に反映されます。
    /// </summary>
    /// <param name="linear_damping">速度の減衰</param>
    void SetLinearDamping(float linear_damping);
    /// <summary>
    /// 角速度の減衰を設定します。次のシミュレーションの前に反映されます。
    /// </summary>
    /// <param name="angular_damping">角速度の減衰</param>
    void SetAngularDamping(float angular_damping);
    /// <summary>
    /// Kinematicにするかどうかを設定し、Physicsに登録し直します。
    /// </summary>
    /// <param name="next_kinematic">Kinematicにする場合 true</param>
    void SetKinematic(bool next_kinematic);
    /// <summary>
    /// Staticにするかどうかを設定し、Physicsに登録し直します。
    /// </summary>
    /// <param name="next_static">Staticにする場合 true</param>
    void SetStatic(bool next_static);
    /// <summary>
    /// 重力の影響を受けるかどうかを設定します。次のシミュレーションの前に反映されます。
    /// </summary>
    /// <param name="use_gravity">重力の影響を受ける場合 true</param>
    void SetGravity(bool use_gravity);
    /// <summary>
    /// 回転を固定する軸を設定します。次のシミュレーションの前に反映されます。
    /// </summary>
    /// <param name="axis">固定する軸</param>
    void SetLockAxis(kLockAxis axis);

    /// <summary>
    /// 重心に力を加えます。
    /// </summary>
    /// <param name="force">加える力</param>
    /// <param name="mode">kForceの場合は力、kImpulseの場合は力積として加えます。</param>
    void AddForce(const Vector3 &force, kForceMode mode = kForceMode::kForce);
    /// <summary>
    /// 指定された位置に力を加えます。
    /// </summary>
    /// <param name="force">加える力</param>
    /// <param name="world_position">力を加えるWorld空間での位置</param>
    /// <param name="mode">kForceの場合は力、kImpulseの場合は力積として加えます。</param>
    void AddForceAtPosition(const Vector3 &force, const Vector3 &world_position, kForceMode mode = kForceMode::kForce);
    /// <summary>
    /// トルクを加えます。
    /// </summary>
    /// <param name="torque">加えるトルク</param>
    /// <param name="mode">kForceの場合はトルク、kImpulseの場合は力積として加えます。</param>
    void AddTorque(const Vector3 &torque, kForceMode mode = kForceMode::kForce);
    /// <summary>
    /// 加えられている力をすべて取り除きます。
    /// </summary>
    void ClearForces() const;

    /// <summary>
    /// KinematicとStaticを両方とも解除します。
    /// </summary>
    void MakeDynamic();
    /// <summary>
    /// スリープ状態を解除します。
    /// </summary>
    void WakeUp() const;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_transform_),
            CEREAL_NVP(m_rigidbody_shape_),
            CEREAL_NVP(m_ghost_shape_),
            CEREAL_NVP(m_velocity_),
            CEREAL_NVP(m_angular_velocity_),
            CEREAL_NVP(m_center_of_mass_),
            CEREAL_NVP(m_mass_),
            CEREAL_NVP(m_linear_damping_),
            CEREAL_NVP(m_angular_damping_),
            CEREAL_NVP(m_friction_),
            CEREAL_NVP(m_rolling_friction_),
            CEREAL_NVP(m_spinning_friction_),
            CEREAL_NVP(m_bounciness_),
            CEREAL_NVP(m_is_kinematic_),
            CEREAL_NVP(m_is_static_),
            CEREAL_NVP(m_use_gravity_)
        );

        RegisterToPhysics();
    }
};
}

CEREAL_CLASS_VERSION(engine::RigidbodyComponent, 1)