#pragma once

#include "mesh_renderer.h"
#include "Rendering/line_renderer.h"

namespace engine
{
class SkinnedMeshRenderer : public MeshRenderer
{
    bool m_draw_bones_ = false;

public:
    constexpr static int kLimitBonesPerVertex = 4;
    std::vector<Matrix> transforms;
    std::vector<std::weak_ptr<MaterialData<std::vector<Matrix>>>> material_bone_matrices_buffers;

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