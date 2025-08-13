#include "pch.h"

#include "rigidbody_component.h"

#include "collider.h"
#include "physics.h"
#include "game_object.h"
#include "gui.h"
#include "Components/transform.h"

namespace engine
{

void RigidbodyComponent::ConstructRigidbody()
{
    m_transform_ = GameObject()->Transform();

    if (m_bt_motion_state_ == nullptr)
    {
        const auto transform = GameObject()->Transform();
        const auto pos = transform->Position();
        const auto rot = transform->Rotation();

        auto starting_transform = btTransform();
        starting_transform.setIdentity();
        starting_transform.setOrigin(btVector3{pos.x, pos.y, pos.z});
        starting_transform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

        m_bt_motion_state_ = std::make_unique<btDefaultMotionState>(starting_transform);
    }

    if (m_bt_compound_shape_ == nullptr)
    {
        m_bt_compound_shape_ = std::make_unique<btCompoundShape>();
        for (auto &weak_collider : m_colliders_)
        {
            AddColliderToCompoundShape(weak_collider.lock());
        }
    }

    if (m_bt_rigidbody_ == nullptr)
    {
        auto ctor_info = btRigidBody::btRigidBodyConstructionInfo(
            IsKinematicOrStatic() ? 0 : m_mass_, m_bt_motion_state_.get(), m_bt_compound_shape_.get());
        m_bt_rigidbody_ = std::make_unique<btRigidBody>(ctor_info);
        m_bt_rigidbody_->setUserPointer(this);
        m_is_registered_ = false;
        m_should_reconstruct_ = false;

        WriteToPhysics();
    }
}

void RigidbodyComponent::RegisterToPhysics()
{
    if (m_is_registered_)
    {
        UnregisterFromPhysics();
    }

    ConstructRigidbody();
    Physics::AddRigidbody(shared_from_base<RigidbodyComponent>());
    m_is_registered_ = true;
}

void RigidbodyComponent::ReadFromPhysics()
{
    btTransform bt_transform = m_bt_rigidbody_->getWorldTransform();

    const auto pos = bt_transform.getOrigin();
    const auto rot = bt_transform.getRotation();
    const auto transform = Transform();
    transform->SetPositionAndRotation({pos.x(), pos.y(), pos.z()}, {rot.x(), rot.y(), rot.z(), rot.w()});
    m_last_world_matrix_ = transform->WorldMatrix();

    const auto bt_vel = m_bt_rigidbody_->getLinearVelocity();
    const auto bt_ang_vel = m_bt_rigidbody_->getAngularVelocity();

    m_velocity_ = Vector3{bt_vel.x(), bt_vel.y(), bt_vel.z()};
    m_angular_velocity_ = Vector3{bt_ang_vel.x(), bt_ang_vel.y(), bt_ang_vel.z()};
}

void RigidbodyComponent::WriteToPhysics()
{
    UpdateCompoundShape();

    const auto transform = GameObject()->Transform();
    const auto pos = transform->Position();
    const auto rot = transform->Rotation();
    const auto center_of_mass = Vector3::Transform(m_center_of_mass_, transform->LocalToWorld());

    btTransform bt_transform = btTransform::getIdentity();
    bt_transform.setOrigin({pos.x, pos.y, pos.z});
    bt_transform.setRotation({rot.x, rot.y, rot.z, rot.w});

    m_bt_rigidbody_->setWorldTransform(bt_transform);
    m_bt_rigidbody_->setLinearVelocity({m_velocity_.x, m_velocity_.y, m_velocity_.z});
    m_bt_rigidbody_->setAngularVelocity({m_angular_velocity_.x, m_angular_velocity_.y, m_angular_velocity_.z});
    m_bt_rigidbody_->setFriction(m_friction_);
    m_bt_rigidbody_->setRollingFriction(m_rolling_friction_);
    m_bt_rigidbody_->setSpinningFriction(m_spinning_friction_);
    m_bt_rigidbody_->setRestitution(m_bounciness_);
    m_bt_rigidbody_->setDamping(m_linear_damping_, m_angular_damping_);

    bt_transform.setOrigin({center_of_mass.x, center_of_mass.y, center_of_mass.z});
    m_bt_rigidbody_->setCenterOfMassTransform(bt_transform);

    btVector3 inertia;
    m_bt_compound_shape_->calculateLocalInertia(m_mass_, inertia);
    m_bt_rigidbody_->setMassProps(IsKinematicOrStatic() ? 0 : m_mass_, inertia);
    m_bt_rigidbody_->setGravity(IsKinematicOrStatic()
                                    ? btVector3{0, 0, 0}
                                    : btVector3{Physics::Gravity().x, Physics::Gravity().y, Physics::Gravity().z});

    m_should_write_ = false;
}

void RigidbodyComponent::UnregisterFromPhysics()
{
    Physics::RemoveRigidbody(shared_from_base<RigidbodyComponent>());
    m_bt_rigidbody_.reset();
    m_is_registered_ = false;
}

void RigidbodyComponent::UpdateCompoundShape()
{
    for (auto &weak_collider : m_colliders_)
    {
        auto collider = weak_collider.lock();
        if (collider == nullptr || !collider->m_is_dirty_)
            continue;

        RemoveColliderFromCompoundShape(collider);
        AddColliderToCompoundShape(collider);
    }
}

void RigidbodyComponent::AddCollider(const std::shared_ptr<Collider> &collider)
{
    m_colliders_.emplace_back(collider);
    AddColliderToCompoundShape(collider);
}

void RigidbodyComponent::RemoveCollider(const std::shared_ptr<Collider> &collider)
{
    erase_if(m_colliders_, [&](auto a) {
        return a.lock() == collider;
    });
    RemoveColliderFromCompoundShape(collider);
}

void RigidbodyComponent::AddColliderToCompoundShape(const std::shared_ptr<Collider> &collider)
{
    const auto collider_matrix = collider->GameObject()->Transform()->WorldMatrix();
    const auto this_matrix = GameObject()->Transform()->WorldMatrix();
    auto relative_matrix = collider_matrix * this_matrix.Invert();

    Vector3 pos, sca;
    Quaternion rot;
    relative_matrix.Decompose(sca, rot, pos);

    btTransform bt_transform = btTransform::getIdentity();
    bt_transform.setOrigin({pos.x, pos.y, pos.z});
    bt_transform.setRotation({rot.x, rot.y, rot.z, rot.w});

    m_bt_compound_shape_->addChildShape(bt_transform, collider->GetShape());
    collider->m_is_dirty_ = false;
    m_should_write_ = true;

    Logger::Log<RigidbodyComponent>("Collider %s has been added at {%.2f, %.2f, %.2f}",
                                    collider->GameObject()->Name().c_str(), pos.x, pos.y, pos.z);
}

void RigidbodyComponent::RemoveColliderFromCompoundShape(const std::shared_ptr<Collider> &collider)
{
    m_bt_compound_shape_->removeChildShape(collider->GetShape());
    m_should_write_ = true;

    Logger::Log<RigidbodyComponent>("Collider %s has been removed", collider->GameObject()->Name().c_str());
}

void RigidbodyComponent::OnEnabled()
{
    RegisterToPhysics();
}

void RigidbodyComponent::OnUpdate()
{
    if (!m_should_write_ && Transform()->WorldMatrix() != m_last_world_matrix_)
    {
        Logger::Log<RigidbodyComponent>("Rigidbody has been moved");
        m_should_write_ = true;
    }

    if (m_should_reconstruct_)
    {
        RegisterToPhysics();
    }
}

void RigidbodyComponent::OnDisabled()
{
    UnregisterFromPhysics();
}

void RigidbodyComponent::OnDestroy()
{
    UnregisterFromPhysics();
}

void RigidbodyComponent::OnInspectorGui()
{
    if (Gui::BoolField("Is Static", m_is_static_))
    {
        SetStatic(m_is_static_);
    }

    if (Gui::BoolField("Is Kinematic", m_is_kinematic_))
    {
        SetKinematic(m_is_kinematic_);
    }

    if (Gui::FloatField("Mass", m_mass_))
    {
        SetMass(m_mass_);
    }

    if (Gui::Vector3Field("Center of Mass", m_center_of_mass_))
    {
        SetCenterOfMass(m_center_of_mass_);
    }

    if (Gui::Vector3Field("Velocity", m_velocity_))
    {
        SetVelocity(m_velocity_);
    }

    if (Gui::Vector3Field("Angular Velocity", m_angular_velocity_))
    {
        SetAngularVelocity(m_angular_velocity_);
    }

    if (ImGui::CollapsingHeader("Advanced Settings"))
    {
        if (Gui::FloatField("Friction", m_friction_))
        {
            SetFriction(m_friction_);
        }

        if (Gui::FloatField("Rolling Friction", m_rolling_friction_))
        {
            SetRollingFriction(m_rolling_friction_);
        }

        if (Gui::FloatField("Spinning Friction", m_spinning_friction_))
        {
            SetSpinningFriction(m_spinning_friction_);
        }

        if (Gui::FloatField("Bounciness", m_bounciness_))
        {
            SetBounciness(m_bounciness_);
        }

        if (Gui::FloatField("Linear Damping", m_linear_damping_))
        {
            SetLinearDamping(m_linear_damping_);
        }

        if (Gui::FloatField("Angular Damping", m_angular_damping_))
        {
            SetAngularDamping(m_angular_damping_);
        }
    }

    if (ImGui::Button("WakeUp"))
    {
        WakeUp();
    }

    if (ImGui::CollapsingHeader("Physics Data", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent();
        if (m_bt_rigidbody_ == nullptr)
        {
            ImGui::Text("Rigidbody has not been setup.");
            ImGui::Unindent();
            return;
        }

        btTransform bt_transform;
        m_bt_motion_state_->getWorldTransform(bt_transform);
        const auto origin = bt_transform.getOrigin();
        const auto rot = bt_transform.getRotation();
        const auto vel = m_bt_rigidbody_->getLinearVelocity();
        const auto ang_vel = m_bt_rigidbody_->getAngularVelocity();
        const auto total_force = m_bt_rigidbody_->getTotalForce();
        const auto total_torque = m_bt_rigidbody_->getTotalTorque();
        const auto com = m_bt_rigidbody_->getCenterOfMassPosition();
        const auto gravity = m_bt_rigidbody_->getGravity();
        const auto inertia = m_bt_rigidbody_->getLocalInertia();
        const auto ang_factor = m_bt_rigidbody_->getAngularFactor();

        ImGui::BulletText("Is Sleeping : %d", IsSleeping());
        ImGui::Text("Rigidbody");
        ImGui::Indent();
        ImGui::Text("Position : {%.2f, %.2f, %.2f}", origin.x(), origin.y(), origin.z());
        ImGui::Text("Rotation : {%.2f, %.2f, %.2f, %.2f}", rot.x(), rot.y(), rot.z(), rot.w());
        ImGui::Text("Velocity    : {%.2f, %.2f, %.2f}", vel.x(), vel.y(), vel.z());
        ImGui::Text("Ang Velocity: {%.2f, %.2f, %.2f}", ang_vel.x(), ang_vel.y(), ang_vel.z());
        ImGui::Text("Total Force : {%.2f, %.2f, %.2f}", total_force.x(), total_force.y(), total_force.z());
        ImGui::Text("Total Torque: {%.2f, %.2f, %.2f}", total_torque.x(), total_torque.y(), total_torque.z());
        ImGui::Text("Ang Factor  : {%.2f, %.2f, %.2f}", ang_factor.x(), ang_factor.y(), ang_factor.z());
        ImGui::Text("Inertia     : {%.2f, %.2f, %.2f}", inertia.x(), inertia.y(), inertia.z());
        ImGui::Text("Center of Mass : {%.2f, %.2f, %.2f}", com.x(), com.y(), com.z());
        ImGui::Text("Gravity  : {%.2f, %.2f, %.2f}", gravity.x(), gravity.y(), gravity.z());
        ImGui::Text("Mass     : %.2f", m_bt_rigidbody_->getMass());
        ImGui::Text("Friction : %.2f", m_bt_rigidbody_->getFriction());
        ImGui::Text("Rolling Friction : %.2f", m_bt_rigidbody_->getRollingFriction());
        ImGui::Text("Spinning Friction: %.2f", m_bt_rigidbody_->getSpinningFriction());
        ImGui::Text("Restitution(Bounciness): %.2f", m_bt_rigidbody_->getRestitution());
        ImGui::Text("Ang Damping: %.2f", m_bt_rigidbody_->getAngularDamping());
        ImGui::Text("Lin Damping: %.2f", m_bt_rigidbody_->getLinearDamping());
        ImGui::Text("Flags    : %d", m_bt_rigidbody_->getFlags());
        ImGui::Text("Is In World              : %d", m_bt_rigidbody_->isInWorld());
        ImGui::Text("Deactivation Time        : %f", m_bt_rigidbody_->getDeactivationTime());
        ImGui::Text("Is Static Or Kinematic   : %d", m_bt_rigidbody_->isStaticOrKinematicObject());
        ImGui::Text("Activation State         : %d", m_bt_rigidbody_->getActivationState());
        ImGui::Unindent();

        const auto shape = m_bt_rigidbody_->getCollisionShape();

        ImGui::Text("Shape");
        ImGui::Indent();
        ImGui::BulletText("Is Compound  : %d", shape->isCompound());
        ImGui::BulletText("Is Non-Moving: %d", shape->isNonMoving());
        ImGui::BulletText("Is Concave   : %d", shape->isConcave());
        ImGui::BulletText("Is Convex    : %d", shape->isConvex());
        ImGui::BulletText("Is Infinite  : %d", shape->isInfinite());
        ImGui::Unindent();

        ImGui::Unindent();
    }
}

std::shared_ptr<Transform> RigidbodyComponent::Transform()
{
    auto lock = m_transform_.lock();
    if (lock == nullptr)
    {
        lock = GameObject()->Transform();
        m_transform_ = lock;
    }

    return lock;
}

Vector3 RigidbodyComponent::Velocity() const
{
    return m_velocity_;
}

Vector3 RigidbodyComponent::AngularVelocity() const
{
    return m_angular_velocity_;
}

Vector3 RigidbodyComponent::CenterOfMass() const
{
    return m_center_of_mass_;
}

float RigidbodyComponent::Mass() const
{
    return m_mass_;
}

float RigidbodyComponent::Friction() const
{
    return m_friction_;
}

float RigidbodyComponent::RollingFriction() const
{
    return m_rolling_friction_;
}

float RigidbodyComponent::SpinningFriction() const
{
    return m_spinning_friction_;
}

float RigidbodyComponent::Bounciness() const
{
    return m_bounciness_;
}

float RigidbodyComponent::LinearDamping() const
{
    return m_linear_damping_;
}

float RigidbodyComponent::AngularDamping() const
{
    return m_angular_damping_;
}

bool RigidbodyComponent::IsSleeping() const
{
    return m_bt_rigidbody_ == nullptr || m_bt_rigidbody_->wantsSleeping();
}

bool RigidbodyComponent::IsKinematic() const
{
    return m_is_kinematic_;
}

bool RigidbodyComponent::IsStatic() const
{
    return m_is_static_;
}

bool RigidbodyComponent::IsKinematicOrStatic() const
{
    return IsKinematic() || IsStatic();
}

bool RigidbodyComponent::IsDynamic() const
{
    return !IsKinematicOrStatic();
}

void RigidbodyComponent::SetPosition(const Vector3 &position)
{
    Transform()->SetPosition(position);
    m_should_write_ = true;
}

void RigidbodyComponent::SetRotation(const Quaternion &rotation)
{
    Transform()->SetRotation(rotation);
    m_should_write_ = true;
}

void RigidbodyComponent::SetVelocity(const Vector3 &velocity)
{
    m_velocity_ = velocity;
    m_should_write_ = true;
    WakeUp();
}

void RigidbodyComponent::SetAngularVelocity(const Vector3 &angular_velocity)
{
    m_angular_velocity_ = angular_velocity;
    m_should_write_ = true;
    WakeUp();
}

void RigidbodyComponent::SetCenterOfMass(const Vector3 &center_of_mass)
{
    m_center_of_mass_ = center_of_mass;
    m_should_write_ = true;
}

void RigidbodyComponent::SetMass(const float mass)
{
    m_mass_ = mass;
    m_should_write_ = true;
}

void RigidbodyComponent::SetFriction(const float friction)
{
    m_friction_ = friction;
    m_should_write_ = true;
}

void RigidbodyComponent::SetRollingFriction(const float rolling_friction)
{
    m_rolling_friction_ = rolling_friction;
    m_should_write_ = true;
}

void RigidbodyComponent::SetSpinningFriction(const float spinning_friction)
{
    m_spinning_friction_ = spinning_friction;
    m_should_write_ = true;
}

void RigidbodyComponent::SetBounciness(const float bounciness)
{
    m_bounciness_ = bounciness;
    m_should_write_ = true;
}

void RigidbodyComponent::SetLinearDamping(const float linear_damping)
{
    m_linear_damping_ = linear_damping;
    m_should_write_ = true;
}

void RigidbodyComponent::SetAngularDamping(const float angular_damping)
{
    m_angular_damping_ = angular_damping;
    m_should_write_ = true;
}

void RigidbodyComponent::SetKinematic(const bool next_kinematic)
{
    m_is_kinematic_ = next_kinematic;
    m_should_reconstruct_ = true;
    m_should_write_ = true;
}

void RigidbodyComponent::SetStatic(const bool next_static)
{
    m_is_static_ = next_static;
    m_should_reconstruct_ = true;
    m_should_write_ = true;
}

void RigidbodyComponent::AddForce(const Vector3 &force, const kForceMode mode)
{
    switch (mode)
    {
    case kForceMode::kForce: // NOLINT(bugprone-branch-clone)
        m_bt_rigidbody_->applyCentralForce({force.x, force.y, force.z});
        break;
    case kForceMode::kImpulse:
        m_bt_rigidbody_->applyCentralImpulse({force.x, force.y, force.z});
        break;
    }

    ReadFromPhysics();
    WakeUp();
}

void RigidbodyComponent::AddForceAtPosition(const Vector3 &force, const Vector3 &world_position, const kForceMode mode)
{
    auto rel = Vector3::Transform(world_position, m_transform_.lock()->WorldToLocal());
    switch (mode)
    {
    case kForceMode::kForce: // NOLINT(bugprone-branch-clone)
        m_bt_rigidbody_->applyForce({force.x, force.y, force.z}, {rel.x, rel.y, rel.z});
        break;
    case kForceMode::kImpulse:
        m_bt_rigidbody_->applyImpulse({force.x, force.y, force.z}, {rel.x, rel.y, rel.z});
        break;
    }

    ReadFromPhysics();
    WakeUp();
}

void RigidbodyComponent::AddTorque(const Vector3 &torque, const kForceMode mode)
{
    switch (mode)
    {
    case kForceMode::kForce: // NOLINT(bugprone-branch-clone)
        m_bt_rigidbody_->applyTorque({torque.x, torque.y, torque.z});
        break;
    case kForceMode::kImpulse:
        m_bt_rigidbody_->applyTorqueImpulse({torque.x, torque.y, torque.z});
        break;
    }

    ReadFromPhysics();
    WakeUp();
}

void RigidbodyComponent::ClearForces() const
{
    m_bt_rigidbody_->clearForces();
    WakeUp();
}

void RigidbodyComponent::MakeDynamic()
{
    SetKinematic(false);
    SetStatic(false);
}

void RigidbodyComponent::WakeUp() const
{
    m_bt_rigidbody_->activate(true);
}
}

CEREAL_REGISTER_TYPE(engine::RigidbodyComponent)