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

    m_debug_drawer_ = std::make_unique<BulletDebugDrawer>();
    m_world_->setDebugDrawer(m_debug_drawer_.get());
    m_world_->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints);
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

        rb->WriteToPhysics();
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

        rb->ReadFromPhysics();
    }
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

void Physics::AddRigidbody(const std::shared_ptr<RigidbodyComponent> &rb)
{
    m_instance_->m_world_->addRigidBody(rb->m_bt_rigidbody_.get());
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
    std::erase_if(m_instance_->m_rigidbodies_, [&](auto a) {
        return a.lock() == rb;
    });

    Logger::Log<Physics>("Rigidbody %s has been removed", rb->GameObject()->Name().c_str());
}
}