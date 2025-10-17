#pragma once
#include "renderer_2d.h"
#include "Rendering/CabotEngine/Graphics/IndexBuffer.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"

namespace engine
{
class Image : public Renderer2D
{
    std::shared_ptr<VertexBuffer> m_vertex_buffer_[RenderEngine::kFrame_Buffer_Count];
    std::shared_ptr<IndexBuffer> m_index_buffer_;
    std::shared_ptr<DescriptorHandle> m_texture_handle_;
    AssetPtr<Texture2D> m_texture_;

public:
    void OnInspectorGui() override;
    void OnAwake() override;
    void OnUpdate() override;
    void Render() override;

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<Renderer2D>(this), CEREAL_NVP(m_texture_));
    }
};
}