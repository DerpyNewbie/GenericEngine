#include "pch.h"
#include "line_renderer.h"

void engine::line_renderer::SetVertices(std::vector<Vertex> vertices)
{
        m_LineRenderer.UpdateVertexBuffer(vertices);
}

void engine::line_renderer::SetIndices(std::vector<uint32_t> indices)
{
    m_LineRenderer.UpdateIndexBuffer(indices);
}

void engine::line_renderer::OnInspectorGui()
{
    Renderer::OnInspectorGui();
}

void engine::line_renderer::OnDraw()
{
    m_LineRenderer.Draw();
}