#include "pch.h"
#include "line_renderer.h"

void engine::LineRendererComponent::SetVertices(std::vector<Vertex> vertices)
{
    m_LineRenderer.UpdateVertexBuffer(vertices);
}

void engine::LineRendererComponent::SetIndices(std::vector<uint32_t> indices)
{
    m_LineRenderer.UpdateIndexBuffer(indices);
}

void engine::LineRendererComponent::OnInspectorGui()
{
    Renderer::OnInspectorGui();
}

void engine::LineRendererComponent::OnDraw()
{
    m_LineRenderer.Draw();
}