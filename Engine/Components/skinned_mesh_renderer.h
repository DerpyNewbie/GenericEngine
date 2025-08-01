#pragma once

#include "mesh_renderer.h"
#include "Asset/asset_ptr.h"

namespace engine
{
class Transform;

class SkinnedMeshRenderer : public MeshRenderer
{
    bool m_draw_bones_ = false;
    std::array<std::shared_ptr<StructuredBuffer>, RenderEngine::FRAME_BUFFER_COUNT> m_bone_matrix_buffers_;

    void DrawBones();
    void UpdateBoneTransformsBuffer();
    std::weak_ptr<Transform> BoundsOrigin() override;

protected:
    void UpdateWVPBuffer() override;

public:
    constexpr static int kMaxBonesPerVertex = 4;
    std::vector<std::weak_ptr<Transform>> transforms;
    AssetPtr<Transform> root_bone;

    void OnInspectorGui() override;

    void OnDraw() override;
    void ReconstructBuffer() override;
    void UpdateBuffers() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<MeshRenderer>(this), CEREAL_NVP(shared_mesh));
    }
};
}