#pragma once

#include "mesh_renderer.h"
#include "Asset/asset_ptr.h"

namespace engine
{
class Transform;

class SkinnedMeshRenderer : public MeshRenderer
{
    bool m_draw_bones_ = false;

    void UpdateBoneTransformsBuffer();
    void DrawBones();

protected:
    void UpdateWVPBuffer() override;

public:
    constexpr static int kLimitBonesPerVertex = 4;
    std::vector<std::weak_ptr<Transform>> transforms;
    std::vector<std::weak_ptr<MaterialData<std::vector<Matrix>>>> bone_transform_buffers;
    AssetPtr<Transform> root_bone;

    void OnInspectorGui() override;

    void OnDraw() override;
    void ReconstructBuffers() override;
    void ReconstructMaterialBuffers(int material_idx) override;
    void UpdateBuffers() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<MeshRenderer>(this), CEREAL_NVP(shared_mesh));
    }
};
}