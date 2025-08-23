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
    std::shared_ptr<Transform> BoundsOrigin() override;

protected:
    static bool m_draw_bounds_;

    std::shared_ptr<Mesh> m_shared_mesh_;
    std::shared_ptr<VertexBuffer> m_vertex_buffer_;
    std::vector<std::shared_ptr<IndexBuffer>> m_index_buffers_;
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::FRAME_BUFFER_COUNT> m_world_matrix_buffers_;

    virtual void ReconstructBuffer();
    virtual Matrix WorldMatrix();

    void UpdateWorldBuffer();

    void DrawBounds();
    void RecalculateBoundingBox();
    void ReconstructMeshesBuffer();
    void SetDescriptorTable(ID3D12GraphicsCommandList *cmd_list, int material_idx);

public:
    std::vector<AssetPtr<Material>> shared_materials;

    bool buffer_creation_failed = false;

    void OnInspectorGui() override;
    void OnDraw() override;

    void SetSharedMesh(const std::shared_ptr<Mesh> &mesh);

    std::shared_ptr<Mesh> GetSharedMesh()
    {
        return m_shared_mesh_;
    }

    virtual void UpdateBuffers();

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Component>(this), CEREAL_NVP(m_shared_mesh_), CEREAL_NVP(shared_materials));
    }
};
}