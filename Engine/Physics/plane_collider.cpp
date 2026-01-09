#include "pch.h"

#include "plane_collider.h"

namespace engine
{

void PlaneCollider::UpdateShape()
{
    // NOTE(derpy): Plane has no need to be updated
}

std::shared_ptr<btCollisionShape> PlaneCollider::GetShape()
{
    return m_plane_;
}
}

CEREAL_REGISTER_TYPE(engine::PlaneCollider)