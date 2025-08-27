#pragma once

#include "mesh_renderer.h"
#include "Asset/asset_ptr.h"

namespace engine
{
class Transform;

class SkinnedMeshRenderer : public MeshRenderer
{
    static bool m_draw_bones_;

    std::array<std::shared_ptr<StructuredBuffer>, RenderEngine::kFrame_Buffer_Count> m_bone_matrix_buffers_;

    Matrix WorldMatrix() override;

    void DrawBones() const;
    void UpdateBoneTransformsBuffer() const;
    std::shared_ptr<Transform> BoundsOrigin() override;

public:
    constexpr static int kMaxBonesPerVertex = 4;

    std::vector<std::weak_ptr<Transform>> transforms;
    std::vector<Matrix> inverted_bind_poses;

    AssetPtr<Transform> root_bone;

    void OnInspectorGui() override;

    void OnDraw() override;
    void ReconstructBuffer() override;
    void UpdateBuffers() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<MeshRenderer>(this),
           CEREAL_NVP(m_shared_mesh_),
           CEREAL_NVP(transforms),
           CEREAL_NVP(inverted_bind_poses));
    }
};
}