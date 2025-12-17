#pragma once
#include "collider.h"
#include "Asset/asset_ptr.h"
#include "Rendering/mesh.h"

namespace engine
{
class MeshCollider : public Collider
{
    AssetPtr<Mesh> m_mesh_;

    void UpdateShape() override;
    btCollisionShape *GetShape() override;
};
}