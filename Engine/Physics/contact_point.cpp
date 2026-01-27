#include "pch.h"
#include "contact_point.h"
#include <BulletCollision/NarrowPhaseCollision/btManifoldPoint.h>

namespace engine
{
Vector3 ContactPoint::Point() const
{
    const btVector3 result = m_is_body_0_ ? m_bt_manifold_point_->getPositionWorldOnA() : m_bt_manifold_point_->getPositionWorldOnB();
    return {result.x(), result.y(), result.z()};
}
Vector3 ContactPoint::Normal() const
{
    const btVector3 result = m_is_body_0_ ? m_bt_manifold_point_->m_normalWorldOnB * -1 : m_bt_manifold_point_->m_normalWorldOnB;
    return {result.x(), result.y(), result.z()};
}
float ContactPoint::Separation() const
{
    return m_bt_manifold_point_->getDistance();
}
}