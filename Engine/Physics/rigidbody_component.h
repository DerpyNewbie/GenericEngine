#pragma once
#include "Components/component.h"
#include "Components/transform.h"

#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet/LinearMath/btMotionState.h>

namespace engine
{
class RigidbodyComponent : public Component
{
    friend class Physics;
    friend class Collider;

    std::unique_ptr<btRigidBody> m_bt_rigidbody_ = nullptr;
    std::unique_ptr<btMotionState> m_bt_motion_state_ = nullptr;
    std::unique_ptr<btCompoundShape> m_bt_compound_shape_ = nullptr;
    std::weak_ptr<Transform> m_transform_ = {};
    std::vector<std::weak_ptr<Collider>> m_colliders_;

    Vector3 m_velocity_ = {};
    bool m_is_dirty_ = true;
    bool m_is_registered_ = false;
    float m_mass_ = 1;
    bool m_is_kinematic_ = true;
    bool m_is_static_ = true;

    void ConstructRigidbody();
    void RegisterToPhysics();
    void ReadFromPhysics();
    void WriteToPhysics();
    void UnregisterFromPhysics();

    void AddCollider(const std::shared_ptr<Collider> &collider);
    void RemoveCollider(const std::shared_ptr<Collider> &collider);
    void AddColliderToCompoundShape(const std::shared_ptr<Collider> &collider) const;
    void RemoveColliderFromCompoundShape(const std::shared_ptr<Collider> &collider) const;

public:
    void OnEnabled() override;
    void OnUpdate() override;
    void OnDisabled() override;
    void OnDestroy() override;

    void OnInspectorGui() override;

    [[nodiscard]] std::shared_ptr<Transform> Transform();
    [[nodiscard]] Vector3 Velocity() const;
    [[nodiscard]] float Mass() const;
    [[nodiscard]] bool Sleeping() const;
    [[nodiscard]] bool IsKinematic() const;
    [[nodiscard]] bool IsStatic() const;
    [[nodiscard]] bool IsKinematicOrStatic() const;
    void SetVelocity(const Vector3 &velocity);
    void SetMass(float mass);
    void SetKinematic(bool next_kinematic);
    void SetStatic(bool next_static);
    void MakeDynamic();
    void Activate() const;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this),
           m_transform_, m_colliders_, m_velocity_, m_mass_, m_is_kinematic_, m_is_static_);

        RegisterToPhysics();
    }
};
}