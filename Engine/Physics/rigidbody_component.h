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
enum class kForceMode : unsigned char
{
    kForce,
    kImpulse
};

class RigidbodyComponent : public Component
{
    friend class Physics;
    friend class Collider;

    std::unique_ptr<btRigidBody> m_bt_rigidbody_ = nullptr;
    std::unique_ptr<btGhostObject> m_bt_ghost_object_ = nullptr;
    std::unique_ptr<btMotionState> m_bt_motion_state_ = nullptr;
    std::unique_ptr<CompoundShape> m_rigidbody_shape_ = nullptr;
    std::unique_ptr<CompoundShape> m_ghost_shape_ = nullptr;

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

    void ConstructRigidbody();
    void RegisterToPhysics();
    void ReadRigidbody();
    void WriteRigidbody();
    void UnregisterFromPhysics();
    void UpdateCompoundShape() const;
    void UpdatePhysics();

    void OnPrePhysicsUpdate();
    void OnPostPhysicsUpdate();

    void AddCollider(const std::shared_ptr<Collider> &collider) const;
    void RemoveCollider(const std::shared_ptr<Collider> &collider) const;

public:
    void OnEnabled() override;
    void OnDisabled() override;
    void OnDestroy() override;

    void OnInspectorGui() override;

    [[nodiscard]] std::shared_ptr<Transform> Transform();
    [[nodiscard]] Vector3 Velocity() const;
    [[nodiscard]] Vector3 AngularVelocity() const;
    [[nodiscard]] Vector3 CenterOfMass() const;
    [[nodiscard]] float Mass() const;
    [[nodiscard]] float Friction() const;
    [[nodiscard]] float RollingFriction() const;
    [[nodiscard]] float SpinningFriction() const;
    [[nodiscard]] float Bounciness() const;
    [[nodiscard]] float LinearDamping() const;
    [[nodiscard]] float AngularDamping() const;
    [[nodiscard]] bool IsSleeping() const;
    [[nodiscard]] bool IsKinematic() const;
    [[nodiscard]] bool IsStatic() const;
    [[nodiscard]] bool IsKinematicOrStatic() const;
    [[nodiscard]] bool IsDynamic() const;

    void SetPosition(const Vector3 &position);
    void SetRotation(const Quaternion &rotation);
    void SetVelocity(const Vector3 &velocity);
    void SetAngularVelocity(const Vector3 &angular_velocity);
    void SetCenterOfMass(const Vector3 &center_of_mass);
    void SetMass(float mass);
    void SetFriction(float friction);
    void SetRollingFriction(float rolling_friction);
    void SetSpinningFriction(float spinning_friction);
    void SetBounciness(float bounciness);
    void SetLinearDamping(float linear_damping);
    void SetAngularDamping(float angular_damping);
    void SetKinematic(bool next_kinematic);
    void SetStatic(bool next_static);

    void AddForce(const Vector3 &force, kForceMode mode = kForceMode::kForce);
    void AddForceAtPosition(const Vector3 &force, const Vector3 &world_position, kForceMode mode = kForceMode::kForce);
    void AddTorque(const Vector3 &torque, kForceMode mode = kForceMode::kForce);
    void ClearForces() const;

    void MakeDynamic();
    void WakeUp() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           CEREAL_NVP(m_transform_), CEREAL_NVP(m_rigidbody_shape_), CEREAL_NVP(m_ghost_shape_),
           CEREAL_NVP(m_velocity_), CEREAL_NVP(m_angular_velocity_), CEREAL_NVP(m_center_of_mass_),
           CEREAL_NVP(m_mass_),
           CEREAL_NVP(m_linear_damping_), CEREAL_NVP(m_angular_damping_),
           CEREAL_NVP(m_friction_), CEREAL_NVP(m_rolling_friction_), CEREAL_NVP(m_spinning_friction_),
           CEREAL_NVP(m_bounciness_),
           CEREAL_NVP(m_is_kinematic_), CEREAL_NVP(m_is_static_));

        RegisterToPhysics();
    }
};
}