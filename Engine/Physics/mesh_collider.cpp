#include "pch.h"
#include "mesh_collider.h"

#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>

#include "gui.h"
#include "Components/mesh_renderer.h"

namespace
{
btVector3 ToBtVec3(const Vector3 vec3)
{
    return btVector3{vec3.x, vec3.y, vec3.z};
}

std::shared_ptr<btTriangleIndexVertexArray> ToTriangleIndexVertexArray(const std::shared_ptr<engine::Mesh> &mesh)
{
    const auto tri_mesh = std::make_shared<btTriangleIndexVertexArray>();
    tri_mesh->preallocateVertices(static_cast<int>(mesh->vertices.size()));
    tri_mesh->preallocateIndices(static_cast<int>(mesh->indices.size()));

    btIndexedMesh indexed_mesh;
    indexed_mesh.m_vertexType = PHY_FLOAT;
    indexed_mesh.m_vertexStride = sizeof(Vector3);
    indexed_mesh.m_vertexBase = reinterpret_cast<const unsigned char *>(mesh->vertices.data());
    indexed_mesh.m_numVertices = static_cast<int>(mesh->vertices.size());

    indexed_mesh.m_triangleIndexStride = sizeof(uint32_t) * 3;
    indexed_mesh.m_triangleIndexBase = reinterpret_cast<const unsigned char *>(mesh->indices.data());
    indexed_mesh.m_numTriangles = static_cast<int>(mesh->indices.size()) / 3;

    tri_mesh->addIndexedMesh(indexed_mesh);
    return tri_mesh;
}

std::shared_ptr<btConvexHullShape> ToBtConvexHullShape(const std::shared_ptr<engine::Mesh> &mesh)
{
    const auto convex_hull = std::make_shared<btConvexHullShape>();
    for (int i = 0; i < mesh->vertices.size(); ++i)
    {
        convex_hull->addPoint(ToBtVec3(mesh->vertices[i]), false);
    }

    convex_hull->recalcLocalAabb();
    return convex_hull;
}
}

namespace engine
{
void MeshCollider::OnInspectorGui()
{
    bool changed = false;
    changed |= Gui::PropertyField("Mesh", m_mesh_);
    changed |= Gui::PropertyField("Use Convex", m_use_convex_);

    if (ImGui::Button("Retrieve mesh from MeshRenderer"))
    {
        const auto mesh_renderer = GameObject()->GetComponent<MeshRenderer>();
        if (mesh_renderer != nullptr)
        {
            m_mesh_ = mesh_renderer->GetSharedMesh();
            changed = true;
        }
    }

    if (changed)
    {
        ApplyChanges();
    }
}

void MeshCollider::SetMesh(const AssetPtr<Mesh> &mesh)
{
    m_mesh_ = mesh;
    ApplyChanges();
}

void MeshCollider::UpdateShape()
{
    const auto mesh = m_mesh_.CastedLock();
    if (mesh == nullptr)
    {
        m_bvh_tri_shape_ = nullptr;
        m_convex_hull_shape_ = nullptr;
        return;
    }

    // NOTE(derpy): there might be a memory leak with tri_mesh handling
    m_tri_mesh_ = ToTriangleIndexVertexArray(mesh);
    m_bvh_tri_shape_ = std::make_shared<btBvhTriangleMeshShape>(m_tri_mesh_.get(), true);
    m_convex_hull_shape_ = ToBtConvexHullShape(mesh);
}

std::shared_ptr<btCollisionShape> MeshCollider::GetShape()
{
    const auto mesh = m_mesh_.CastedLock();
    if (mesh == nullptr)
    {
        return nullptr;
    }

    if (m_use_convex_)
    {
        return m_convex_hull_shape_;
    }

    return m_bvh_tri_shape_;
}
}

CEREAL_REGISTER_TYPE(engine::MeshCollider)