#pragma once
#include "renderer_2d.h"
#include "Asset/asset_ptr.h"
#include "Rendering/material.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class Image : public Renderer2D
{
    std::shared_ptr<ConstantBuffer> m_world_matrix_buffer_;

    void UpdateWorldBuffer();
    
public:
    AssetPtr<Material> shared_material;

    void OnInspectorGui() override;
    void Render() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Renderer2D>(this));
        if (version >= 2)
        {
            ar(shared_material);
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::Image, 2)