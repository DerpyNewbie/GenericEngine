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
    unsigned int m_NumIndices;

    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;
    std::shared_ptr<ConstantBuffer> m_ViewProjectionBuffers[RenderEngine::FRAME_BUFFER_COUNT];
    bool m_IsValid = true;

public:
    void SetVertices(std::vector<Vertex> vertices);
    void SetIndices(std::vector<uint32_t> indices);

    void OnInspectorGui() override;

    void OnDraw() override;

};
}