#pragma once
#include "Vertex.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/IndexBuffer.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/VertexBuffer.h"

class LineRenderer
{
    unsigned int m_NumIndices;

    std::shared_ptr<engine::VertexBuffer> m_VertexBuffer;
    std::shared_ptr<engine::IndexBuffer> m_IndexBuffer;
    std::shared_ptr<ConstantBuffer> m_ViewProjectionBuffers[RenderEngine::FRAME_BUFFER_COUNT];
    bool m_IsValid = true;

public:
    void UpdateVertexBuffer(std::vector<Vertex> vertices);
    void UpdateIndexBuffer(std::vector<uint32_t> indices);
    void UpdateViewProjectionBuffer();
    LineRenderer(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

    bool IsValid()
    {
        return m_IsValid;
    };

    void Draw();
};