#pragma once
#include "bullet_debug_drawer.h"
#include "event_receivers.h"
#include <bullet/btBulletDynamicsCommon.h>

namespace engine
{
class RigidbodyComponent;


class Physics : public IFixedUpdateReceiver
{
    friend class Engine;

    static std::shared_ptr<Physics> m_instance_;
    Vector3 m_gravity_;

    std::unique_ptr<btCollisionConfiguration> m_collision_configuration_;
    std::unique_ptr<btCollisionDispatcher> m_dispatcher_;
    std::unique_ptr<btBroadphaseInterface> m_broadphase_;
    std::unique_ptr<btConstraintSolver> m_solver_;
    std::unique_ptr<btDynamicsWorld> m_world_;
    std::unique_ptr<BulletDebugDrawer> m_debug_drawer_;
    std::vector<std::weak_ptr<RigidbodyComponent>> m_rigidbodies_;

    using ContactPair = std::pair<const RigidbodyComponent *, const RigidbodyComponent *>;
    std::map<ContactPair, const btPersistentManifold *> m_current_contacts_;
    std::map<ContactPair, const btPersistentManifold *> m_previous_contacts_;

    static void Init();

    Physics();

    static void OnCollisionStarted(const ContactPair &pair, const btPersistentManifold *manifold);
    static void OnCollisionStayed(const ContactPair &pair, const btPersistentManifold *manifold);
    static void OnCollisionExited(const ContactPair &pair, const btPersistentManifold *manifold);

public:
    int Order() override;
    void OnFixedUpdate() override;

    static void DebugDraw();
    static Vector3 Gravity();
    static void SetGravity(const Vector3 &gravity);

    static void AddRigidbody(const std::shared_ptr<RigidbodyComponent> &rb);
    static void RemoveRigidbody(const std::shared_ptr<RigidbodyComponent> &rb);
};
}