#include "pch.h"
#include "mesh_collider.h"

namespace engine
{
void MeshCollider::UpdateShape() {

}
btCollisionShape *MeshCollider::GetShape()
{
    const auto mesh = m_mesh_.CastedLock();
    if (mesh == nullptr)
    {
        return nullptr;
    }

    return nullptr;
}
}