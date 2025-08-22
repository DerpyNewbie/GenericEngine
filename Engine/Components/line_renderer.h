#pragma once
#include "renderer.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/CabotEngine/Graphics/IndexBuffer.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/VertexBuffer.h"

namespace engine
{
class LineRenderer : public Renderer
{
    unsigned int m_num_indices_ = 0;

    std::shared_ptr<VertexBuffer> m_vertex_buffer_;
    std::shared_ptr<IndexBuffer> m_index_buffer_;
    std::shared_ptr<ConstantBuffer> m_view_projection_buffers_[RenderEngine::kFrame_Buffer_Count];
    bool m_is_valid_ = true;

public:
    void SetVertices(std::vector<Vertex> vertices);
    void SetIndices(std::vector<uint32_t> indices);

    void OnInspectorGui() override;

    void OnDraw() override;

};
}