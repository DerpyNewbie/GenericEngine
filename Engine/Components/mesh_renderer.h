#pragma once
#include "component.h"
#include "renderer.h"
#include "Rendering/buffer_data_base.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
class MeshRenderer : public Renderer
{
    Matrix BoundsOrigin() override;
    virtual void UpdateWorldBuffer();

protected:
    static bool m_draw_bounds_;
    bool m_cast_shadow_ = true;

    AssetPtr<Material> m_shadow_material_;
    AssetPtr<Mesh> m_shared_mesh_;
    std::shared_ptr<ConstantBuffer> m_world_matrix_buffer_;

    void DrawBounds();
    void RecalculateBoundingBox();

public:

    uint32_t instance_count = 1;
    bool buffer_creation_failed = false;

    void OnInspectorGui() override;
    void DepthRender() override;
    void Render() override;

    AssetPtr<Material> GetShadowMaterial();
    
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
            CEREAL_NVP(m_shared_mesh_)
        );

        if (version >= 3)
        {
            ar(CEREAL_NVP(instance_count));
        }

        if (version >= 4)
        {
            ar(CEREAL_NVP(m_cast_shadow_));
        }

        if (version >= 5)
        {
            ar(CEREAL_NVP(m_shadow_material_));
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::MeshRenderer, 5)