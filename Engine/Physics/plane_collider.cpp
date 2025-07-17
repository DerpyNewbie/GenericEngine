#include "pch.h"

#include "plane_collider.h"

namespace engine
{

void PlaneCollider::UpdateShape()
{}
btCollisionShape *PlaneCollider::GetShape()
{
    return &m_plane_;
}
}

CEREAL_REGISTER_TYPE(engine::PlaneCollider)