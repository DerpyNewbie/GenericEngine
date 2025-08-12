#pragma once
#include "bullet_debug_drawer.h"
#include "event_receivers.h"
#include "Components/component.h"

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
    using CollisionPair = std::pair<Collision, Collision>;
    std::map<ContactPair, CollisionPair> m_current_contacts_;
    std::map<ContactPair, CollisionPair> m_previous_contacts_;

    static void Init();

    Physics();

    static Vector3 CalculateNormalFromManifold(btPersistentManifold *manifold);

    static void OnCollisionStarted(const ContactPair &contact_pair, const CollisionPair &collision_pair);
    static void OnCollisionStayed(const ContactPair &contact_pair, const CollisionPair &collision_pair);
    static void OnCollisionExited(const ContactPair &contact_pair, const CollisionPair &collision_pair);

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