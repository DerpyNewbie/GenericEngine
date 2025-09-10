#include "pch.h"

#include "physics.h"

#include "game_object.h"
#include "engine_time.h"
#include "rigidbody_component.h"
#include "update_manager.h"

namespace engine
{
std::shared_ptr<Physics> Physics::m_instance_;

void Physics::Init()
{
    if (m_instance_ != nullptr)
    {
        throw std::runtime_error("Physics has already been initialized.");
    }

    m_instance_ = std::make_shared<Physics>(Physics());
    UpdateManager::SubscribeFixedUpdate(m_instance_);
}

Physics::Physics()
{
    m_gravity_ = Vector3{0, -9.81f, 0};
    m_collision_configuration_ = std::make_unique<btDefaultCollisionConfiguration>();
    m_dispatcher_ = std::make_unique<btCollisionDispatcher>(m_collision_configuration_.get());
    m_broadphase_ = std::make_unique<btDbvtBroadphase>();
    m_solver_ = std::make_unique<btSequentialImpulseConstraintSolver>();
    m_world_ = std::make_unique<btDiscreteDynamicsWorld>(
        m_dispatcher_.get(),
        m_broadphase_.get(),
        m_solver_.get(),
        m_collision_configuration_.get());

    m_world_->setGravity({m_gravity_.x, m_gravity_.y, m_gravity_.z});
    m_world_->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

    m_debug_drawer_ = std::make_unique<BulletDebugDrawer>();
    m_world_->setDebugDrawer(m_debug_drawer_.get());
    m_world_->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints);
}

Vector3 Physics::CalculateNormalFromManifold(btPersistentManifold *manifold)
{
    btVector3 total_normal(0, 0, 0);
    const int num_contacts = manifold->getNumContacts();

    if (num_contacts == 0)
    {
        // No contacts, no normal
        return Vector3::Zero;
    }

    for (int i = 0; i < num_contacts; i++)
    {
        const btManifoldPoint &pt = manifold->getContactPoint(i);

        // Optionally weight by penetration depth (negative distance means penetration)
        float weight = -pt.getDistance(); // use positive penetration depth as weight
        if (weight < 0)
            continue; // ignore if no penetration

        // Add weighted normal
        total_normal += pt.m_normalWorldOnB * weight;
    }

    // Normalize the total vector to get an average direction
    if (!total_normal.fuzzyZero())
    {
        total_normal.normalize();
        return {total_normal.x(), total_normal.y(), total_normal.z()};
    }

    return Vector3::Zero;
}

void Physics::OnCollisionStarted(const ContactPair &contact_pair, const CollisionPair &collision_pair)
{
    Logger::Log<Physics>("Collision Started: %s -> %s",
                         contact_pair.first->GameObject()->Name().c_str(),
                         contact_pair.second->GameObject()->Name().c_str());
    contact_pair.first->GameObject()->InvokeOnCollisionEnter(collision_pair.first);
    contact_pair.second->GameObject()->InvokeOnCollisionEnter(collision_pair.second);
}

void Physics::OnCollisionStayed(const ContactPair &contact_pair, const CollisionPair &collision_pair)
{
    contact_pair.first->GameObject()->InvokeOnCollisionStay(collision_pair.first);
    contact_pair.second->GameObject()->InvokeOnCollisionStay(collision_pair.second);
}

void Physics::OnCollisionExited(const ContactPair &contact_pair, const CollisionPair &collision_pair)
{
    Logger::Log<Physics>("Collision Exited: %s -> %s",
                         contact_pair.first->GameObject()->Name().c_str(),
                         contact_pair.second->GameObject()->Name().c_str());
    contact_pair.first->GameObject()->InvokeOnCollisionExit(collision_pair.first);
    contact_pair.second->GameObject()->InvokeOnCollisionExit(collision_pair.second);
}

void Physics::OnTriggerStarted(const std::shared_ptr<GameObject> &target, const std::shared_ptr<GameObject> &other)
{
    Logger::Log<Physics>("Trigger Started: %s -> %s", target->Name().c_str(), other->Name().c_str());
    target->InvokeOnTriggerEnter(other);
}

void Physics::OnTriggerStayed(const std::shared_ptr<GameObject> &target, const std::shared_ptr<GameObject> &other)
{
    target->InvokeOnTriggerStay(other);
}

void Physics::OnTriggerExited(const std::shared_ptr<GameObject> &target, const std::shared_ptr<GameObject> &other)
{
    Logger::Log<Physics>("Trigger Exited: %s -> %s", target->Name().c_str(), other->Name().c_str());
    target->InvokeOnTriggerExit(other);
}

void Physics::AddTriggerOverlap(const std::shared_ptr<GameObject> &a, const std::shared_ptr<GameObject> &b)
{
    m_instance_->m_current_overlaps_.emplace(std::minmax(a, b));
}

void Physics::AddRigidbody(const std::shared_ptr<RigidbodyComponent> &rb)
{
    m_instance_->m_world_->addRigidBody(rb->m_bt_rigidbody_.get());
    m_instance_->m_world_->addCollisionObject(rb->m_bt_ghost_object_.get(), btBroadphaseProxy::SensorTrigger);
    m_instance_->m_rigidbodies_.emplace_back(rb);

    Logger::Log<Physics>("Rigidbody %s has been added", rb->GameObject()->Name().c_str());
}

void Physics::RemoveRigidbody(const std::shared_ptr<RigidbodyComponent> &rb)
{
    if (!rb->m_is_registered_)
    {
        return;
    }

    m_instance_->m_world_->removeRigidBody(rb->m_bt_rigidbody_.get());
    m_instance_->m_world_->removeCollisionObject(rb->m_bt_ghost_object_.get());

    std::erase_if(m_instance_->m_rigidbodies_, [&](auto a) {
        return a.lock() == rb;
    });

    Logger::Log<Physics>("Rigidbody %s has been removed", rb->GameObject()->Name().c_str());
}

void Physics::ProcessCollisions()
{
    m_current_contacts_.clear();

    const auto dispatcher = m_world_->getDispatcher();
    const int manifolds_count = dispatcher->getNumManifolds();
    for (int i = 0; i < manifolds_count; i++)
    {
        btPersistentManifold *manifold = dispatcher->getManifoldByIndexInternal(i);
        if (manifold->getNumContacts() > 0)
        {
            const btCollisionObject *body_0 = manifold->getBody0();
            const btCollisionObject *body_1 = manifold->getBody1();
            auto rb_0 = static_cast<RigidbodyComponent *>(body_0->getUserPointer());
            auto rb_1 = static_cast<RigidbodyComponent *>(body_1->getUserPointer());
            auto contact_pair = std::minmax(rb_0, rb_1);
            auto normal = CalculateNormalFromManifold(manifold);
            const auto first_is_zero = contact_pair.first == rb_0;
            auto collision_pair = std::make_pair(
                Collision{first_is_zero ? rb_1->GameObject().get() : rb_0->GameObject().get(),
                          first_is_zero ? normal : normal * -1},
                Collision{first_is_zero ? rb_0->GameObject().get() : rb_1->GameObject().get(),
                          first_is_zero ? normal * -1 : normal});

            m_current_contacts_.emplace(contact_pair, collision_pair);

            m_previous_contacts_.contains(contact_pair)
                ? OnCollisionStayed(contact_pair, collision_pair)
                : OnCollisionStarted(contact_pair, collision_pair);
        }
    }

    for (auto &[contact, collision] : m_previous_contacts_)
    {
        if (!m_current_contacts_.contains(contact))
        {
            OnCollisionExited(contact, collision);
        }
    }

    m_previous_contacts_ = m_current_contacts_;
}

void Physics::ProcessTriggers()
{
    for (const auto &pair : m_current_overlaps_)
    {
        if (m_previous_overlaps_.contains(pair))
        {
            OnTriggerStayed(pair.first, pair.second);
        }
        else
        {
            OnTriggerStarted(pair.first, pair.second);
        }
    }

    for (const auto &pair : m_previous_overlaps_)
    {
        if (!m_current_overlaps_.contains(pair))
        {
            OnTriggerExited(pair.first, pair.second);
        }
    }

    m_previous_overlaps_ = m_current_overlaps_;
    m_current_overlaps_ = std::set<TriggerPair>{};
}

int Physics::Order()
{
    return IFixedUpdateReceiver::Order();
}

void Physics::OnFixedUpdate()
{
    const auto rbs = m_rigidbodies_;
    for (auto weak_rb : rbs)
    {
        auto rb = weak_rb.lock();
        if (rb == nullptr)
        {
            Logger::Warn<Physics>("Invalidated Rigidbody has found in Pre Physics FixedUpdate");
            continue;
        }

        rb->OnPrePhysicsUpdate();
    }

    m_world_->stepSimulation(Time::Get()->FixedDeltaTime(), 0);

    for (auto weak_rb : m_rigidbodies_)
    {
        auto rb = weak_rb.lock();
        if (rb == nullptr)
        {
            Logger::Warn<Physics>("Invalidated Rigidbody has found in Post Physics FixedUpdate");
            continue;
        }

        rb->OnPostPhysicsUpdate();
        rb->CollectOverlaps();
    }

    ProcessCollisions();
    ProcessTriggers();
}

void Physics::DebugDraw()
{
    m_instance_->m_world_->debugDrawWorld();
}

Vector3 Physics::Gravity()
{
    return m_instance_->m_gravity_;
}

void Physics::SetGravity(const Vector3 &gravity)
{
    m_instance_->m_gravity_ = gravity;
    m_instance_->m_world_->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

}