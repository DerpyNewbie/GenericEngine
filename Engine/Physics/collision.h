#pragma once
#include "contact_point.h"

class btPersistentManifold;

namespace engine
{
class GameObject;
class Collision
{
    friend class Physics;

    std::weak_ptr<GameObject> m_other_ = {};
    btPersistentManifold *m_manifold_ = nullptr;
    Vector3 m_impulse_ = Vector3::Zero;
    bool m_is_body_0_ = false;

    static Vector3 CalculateNormalFromManifold(btPersistentManifold *manifold);

    Collision(const std::weak_ptr<GameObject> &other, btPersistentManifold *manifold, bool is_body_0);

public:
    [[nodiscard]] std::shared_ptr<GameObject> Other() const;
    [[nodiscard]] size_t ContactCount() const;
    [[nodiscard]] ContactPoint GetContact(int index) const;
    [[nodiscard]] std::vector<ContactPoint> GetContacts() const;
};
}