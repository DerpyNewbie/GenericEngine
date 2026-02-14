#pragma once
#include "renderer_2d.h"
#include "Asset/asset_ptr.h"
#include "Rendering/material.h"
#include "Rendering/CabotEngine/Graphics/IndexBuffer.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"

namespace engine
{
class Image : public Renderer2D
{
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count> m_world_matrix_buffers_;

    void UpdateWorldBuffer();
    
public:
    AssetPtr<Material> shared_material;

    void OnInspectorGui() override;
    void Render() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Renderer2D>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::Image, 1)