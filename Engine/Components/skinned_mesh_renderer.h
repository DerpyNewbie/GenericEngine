#pragma once

#include "mesh_renderer.h"
#include "Asset/asset_ptr.h"

namespace engine
{
class Transform;

class SkinnedMeshRenderer : public MeshRenderer
{
    bool m_draw_bones_ = false;

    void DrawBones();
    void UpdateBoneTransformsBuffer();
    std::weak_ptr<Transform> GetTransform() override;

protected:
    void UpdateWVPBuffer() override;

public:
    constexpr static int kLimitBonesPerVertex = 4;
    std::vector<std::weak_ptr<Transform>> transforms;
    std::array<std::shared_ptr<StructuredBuffer>, RenderEngine::FRAME_BUFFER_COUNT> bone_matrices_buffers;
    AssetPtr<Transform> root_bone;

    void OnInspectorGui() override;

    void OnDraw() override;
    void Reconstruct() override;
    void UpdateBuffers() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<MeshRenderer>(this), CEREAL_NVP(shared_mesh));
    }
};
}