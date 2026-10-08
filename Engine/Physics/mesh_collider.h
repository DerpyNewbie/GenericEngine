#pragma once
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>

#include "collider.h"
#include "Asset/asset_ptr.h"
#include "Rendering/mesh.h"

namespace engine
{
/// <summary>
/// Meshの形状を使うColliderです。
/// </summary>
class MeshCollider : public Collider
{
    AssetPtr<Mesh> m_mesh_;
    bool m_use_convex_ = false;

    std::shared_ptr<btTriangleIndexVertexArray> m_tri_mesh_;
    std::shared_ptr<btBvhTriangleMeshShape> m_bvh_tri_shape_;
    std::shared_ptr<btConvexHullShape> m_convex_hull_shape_;

    void OnInspectorGui() override;

    /// <summary>
    /// 設定に応じて、ConvexHullまたは三角形Meshの形状を取得します。
    /// </summary>
    /// <returns>Meshが設定されていない場合 nullptr</returns>
    std::shared_ptr<btCollisionShape> GetShape() override;
    /// <summary>
    /// Meshから三角形Meshの形状とConvexHullの形状を作り直します。
    /// </summary>
    void UpdateShape() override;

public:
    /// <summary>
    /// 形状に使うMeshを設定し、Rigidbodyに反映します。
    /// </summary>
    /// <param name="mesh">形状に使うMesh</param>
    void SetMesh(const AssetPtr<Mesh> &mesh);

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Collider>(this), CEREAL_NVP(m_mesh_), CEREAL_NVP(m_use_convex_));
    }
};
}

CEREAL_CLASS_VERSION(engine::MeshCollider, 1)