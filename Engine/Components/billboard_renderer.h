#pragma once
#include "renderer.h"
#include "Rendering/CabotEngine/Graphics/billboard.h"

namespace engine
{
class Material;

class BillboardRenderer : public Renderer
{
    std::array<std::shared_ptr<ConstantBuffer>, RenderEngine::kFrame_Buffer_Count> m_world_matrix_buffers_;

    void UpdateWorldBuffer();
    
public:

    void OnConstructed() override;
    void OnInspectorGui() override;
    void Render() override;
    Matrix BoundsOrigin() override;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(cereal::base_class<Renderer>(this));
    }
};
}

CEREAL_CLASS_VERSION(engine::BillboardRenderer, 1)