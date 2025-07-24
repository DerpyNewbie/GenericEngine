#pragma once
#include "pch.h"
#include "component.h"
#include "renderer.h"
#include "Rendering/material_data.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"
#include "Rendering/CabotEngine/Graphics/IndexBuffer.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"

namespace engine
{
class MeshRenderer : public Renderer
{
    bool m_already_calc_bounds_ = false;

    std::weak_ptr<Transform> GetTransform() override;

protected:
    bool m_draw_bounds_ = false;

    virtual void Reconstruct();
    virtual void UpdateWVPBuffer();

    void DrawBounds();
    void CalcBoundingBox();
    void ReconstructMeshesBuffer();
    void SetDescriptorTable(ID3D12GraphicsCommandList *cmd_list, int material_idx);

public:
    std::shared_ptr<Mesh> shared_mesh;
    std::vector<AssetPtr<Material>> shared_materials;

    std::shared_ptr<VertexBuffer> vertex_buffer;
    std::vector<std::shared_ptr<IndexBuffer>> index_buffers;
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::FRAME_BUFFER_COUNT> wvp_buffers;

    bool buffer_creation_failed = false;
    bool is_material_error = false;

    void OnInspectorGui() override;
    void OnUpdate() override;
    void OnDraw() override;

    virtual void UpdateBuffers();

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(shared_mesh), CEREAL_NVP(shared_materials));
    }
};
}