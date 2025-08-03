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
        m_is_dirty_ = false;
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

    Transform()->SetPositionAndRotation({pos.x(), pos.y(), pos.z()}, {rot.x(), rot.y(), rot.z(), rot.w()});

    const auto bt_vel = m_bt_rigidbody_->getLinearVelocity();
    m_velocity_ = Vector3{bt_vel.x(), bt_vel.y(), bt_vel.z()};
}

void RigidbodyComponent::WriteToPhysics()
{
    UpdateCompoundShape();

    const auto transform = GameObject()->Transform();
    const auto pos = transform->Position();
    const auto rot = transform->Rotation();

    btTransform bt_transform = btTransform::getIdentity();
    bt_transform.setOrigin({pos.x, pos.y, pos.z});
    bt_transform.setRotation({rot.x, rot.y, rot.z, rot.w});

    m_bt_rigidbody_->setWorldTransform(bt_transform);
    m_bt_rigidbody_->setLinearVelocity({m_velocity_.x, m_velocity_.y, m_velocity_.z});
    m_bt_rigidbody_->setMassProps(IsKinematicOrStatic() ? 0 : m_mass_, {0, 0, 0});
    m_bt_rigidbody_->setGravity(IsKinematicOrStatic()
                                    ? btVector3{0, 0, 0}
                                    : btVector3{Physics::Gravity().x, Physics::Gravity().y, Physics::Gravity().z});
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

void RigidbodyComponent::AddColliderToCompoundShape(const std::shared_ptr<Collider> &collider) const
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
    Logger::Log<RigidbodyComponent>("Collider %s has been added at {%.2f, %.2f, %.2f}",
                                    collider->GameObject()->Name().c_str(), pos.x, pos.y, pos.z);
}

void RigidbodyComponent::RemoveColliderFromCompoundShape(const std::shared_ptr<Collider> &collider) const
{
    m_bt_compound_shape_->removeChildShape(collider->GetShape());
    Logger::Log<RigidbodyComponent>("Collider %s has been removed", collider->GameObject()->Name().c_str());
}

void RigidbodyComponent::OnEnabled()
{
    RegisterToPhysics();
}
void RigidbodyComponent::OnUpdate()
{
    if (m_is_dirty_)
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
        m_is_dirty_ = true;
    }

    if (Gui::BoolField("Is Kinematic", m_is_kinematic_))
    {
        m_is_dirty_ = true;
    }

    if (Gui::FloatField("Mass", m_mass_))
    {
        m_is_dirty_ = true;
    }

    if (Gui::Vector3Field("Velocity", m_velocity_))
    {
        Logger::Log<RigidbodyComponent>("Velocity set to {%.2f, %.2f, %.2f}",
                                        m_velocity_.x, m_velocity_.y, m_velocity_.z);
        m_is_dirty_ = true;
    }

    if (ImGui::Button("Activate"))
    {
        Activate();
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
        const auto com = m_bt_rigidbody_->getCenterOfMassPosition();
        const auto gravity = m_bt_rigidbody_->getGravity();

        ImGui::BulletText("Sleeping : %d", Sleeping());
        ImGui::Text("Rigidbody");
        ImGui::Indent();
        ImGui::Text("Position : {%.2f, %.2f, %.2f}", origin.x(), origin.y(), origin.z());
        ImGui::Text("Rotation : {%.2f, %.2f, %.2f, %.2f}", rot.x(), rot.y(), rot.z(), rot.w());
        ImGui::Text("Velocity : {%.2f, %.2f, %.2f}", vel.x(), vel.y(), vel.z());
        ImGui::Text("Gravity  : {%.2f, %.2f, %.2f}", gravity.x(), gravity.y(), gravity.z());
        ImGui::Text("Center of Mass : {%.2f, %.2f, %.2f}", com.x(), com.y(), com.z());
        ImGui::Text("Mass     : %.2f", m_bt_rigidbody_->getMass());
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

float RigidbodyComponent::Mass() const
{
    return m_mass_;
}

bool RigidbodyComponent::Sleeping() const
{
    return m_bt_rigidbody_->wantsSleeping();
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

void RigidbodyComponent::SetVelocity(const Vector3 &velocity)
{
    m_velocity_ = velocity;
    m_is_dirty_ = true;
}

void RigidbodyComponent::SetMass(const float mass)
{
    m_mass_ = mass;
    m_is_dirty_ = true;
}
void RigidbodyComponent::SetKinematic(const bool next_kinematic)
{
    m_is_kinematic_ = next_kinematic;
    m_is_dirty_ = true;
}

void RigidbodyComponent::SetStatic(const bool next_static)
{
    m_is_static_ = next_static;
    m_is_dirty_ = true;
}

void RigidbodyComponent::MakeDynamic()
{
    SetKinematic(false);
    SetStatic(false);
}

void RigidbodyComponent::Activate() const
{
    m_bt_rigidbody_->activate(true);
}
}

CEREAL_REGISTER_TYPE(engine::RigidbodyComponent)