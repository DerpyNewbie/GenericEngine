#pragma once

class btManifoldPoint;
class btPersistentManifold;

namespace engine
{
class ContactPoint
{
    friend class Collision;

    const btManifoldPoint *m_bt_manifold_point_;
    bool m_is_body_0_;

    explicit ContactPoint(const btManifoldPoint &bt_manifold_point, const bool is_body_0)
    {
        m_bt_manifold_point_ = &bt_manifold_point;
        m_is_body_0_ = is_body_0;
    }

public:
    [[nodiscard]] Vector3 Point() const;
    [[nodiscard]] Vector3 Normal() const;
    [[nodiscard]] float Separation() const;
};
}