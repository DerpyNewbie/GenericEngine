#include "pch.h"
#include "collision.h"

#include <BulletCollision/NarrowPhaseCollision/btPersistentManifold.h>
#include <BulletCollision/NarrowPhaseCollision/btManifoldPoint.h>

namespace engine
{
Vector3 Collision::CalculateNormalFromManifold(btPersistentManifold *manifold)
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

Collision::Collision(const std::weak_ptr<GameObject> &other, btPersistentManifold *manifold, const bool is_body_0) : m_other_(other), m_manifold_(manifold), m_is_body_0_(is_body_0)
{
    btVector3 total_impulse(0, 0, 0);
    for (int i = 0; i < manifold->getNumContacts(); ++i)
    {
        auto cp = manifold->getContactPoint(i);
        total_impulse += cp.m_normalWorldOnB * cp.getAppliedImpulse();
    }
    m_impulse_ = {total_impulse.x(), total_impulse.y(), total_impulse.z()};
}
std::shared_ptr<GameObject> Collision::Other() const
{
    return m_other_.lock();
}

size_t Collision::ContactCount() const
{
    return m_manifold_->getNumContacts();
}

assert(index >= 0 && index < static_cast<int>(ContactCount()) && "Contact index out of range");

std::vector<ContactPoint> Collision::GetContacts() const
{
    std::vector<ContactPoint> contacts;
    contacts.reserve(ContactCount());
    for (int i = 0; i < ContactCount(); i++)
    {
        contacts.emplace_back(GetContact(i));
    }
    return contacts;
}
}