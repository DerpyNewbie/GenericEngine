#pragma once
#include "component.h"
#include "renderer.h"
#include "Rendering/material_data.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"

namespace engine
{
class MeshRenderer : public Renderer
{
    Matrix BoundsOrigin() override;
    virtual void UpdateWorldBuffer();

protected:
    static bool m_draw_bounds_;
    
    AssetPtr<Mesh> m_shared_mesh_;

    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count> m_world_matrix_buffers_;

    void DrawBounds();
    void RecalculateBoundingBox();

public:

    std::vector<AssetPtr<Material>> shared_materials;
    bool buffer_creation_failed = false;

    void OnInspectorGui() override;
    void DepthRender() override;
    void Render() override;

    void SetSharedMesh(const AssetPtr<Mesh> &mesh);

    AssetPtr<Mesh> GetSharedMesh()
    {
        return m_shared_mesh_;
    }

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Renderer>(this),
            CEREAL_NVP(m_shared_mesh_),
            CEREAL_NVP(shared_materials)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::MeshRenderer, 1)