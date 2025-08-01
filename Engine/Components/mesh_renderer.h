#pragma once
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
    std::weak_ptr<Transform> BoundsOrigin() override;

protected:
    bool m_draw_bounds_ = false;

    std::shared_ptr<Mesh> shared_mesh;
    std::shared_ptr<VertexBuffer> vertex_buffer;
    std::vector<std::shared_ptr<IndexBuffer>> index_buffers;
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::FRAME_BUFFER_COUNT> world_matrix_buffers;

    virtual void ReconstructBuffer();
    virtual void UpdateWVPBuffer();

    void DrawBounds();
    void RecalculateBoundingBox();
    void ReconstructMeshesBuffer();
    void SetDescriptorTable(ID3D12GraphicsCommandList *cmd_list, int material_idx);

public:
    std::vector<AssetPtr<Material>> shared_materials;

    bool buffer_creation_failed = false;

    void OnInspectorGui() override;
    void OnDraw() override;

    void SetSharedMesh(std::shared_ptr<Mesh> mesh);

    std::shared_ptr<Mesh> GetSharedMesh()
    {
        return shared_mesh;
    }

    virtual void UpdateBuffers();

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(shared_mesh), CEREAL_NVP(shared_materials));
    }
};
}