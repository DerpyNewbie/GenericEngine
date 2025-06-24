#pragma once
#include "DxLib.h"
#include "game_object.h"
#include "mesh_renderer.h"
#include "Rendering/line_renderer.h"
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"

#include <vector>

namespace engine
{
class SkinnedMeshRenderer : public MeshRenderer
{
    bool m_draw_bones_ = false;

public:
    constexpr static int kLimitBonesPerVertex = 4;
    std::vector<DirectX::XMMATRIX> transforms;

    StructuredBuffer<DirectX::XMMATRIX> transforms_buffer;
    std::shared_ptr<LineRenderer> lineRenderer;

    void OnInspectorGui() override;

    void OnDraw() override;

    void ReconstructBuffers() override;
    void UpdateBuffers() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<MeshRenderer>(this), CEREAL_NVP(shared_mesh));
    }
};
}