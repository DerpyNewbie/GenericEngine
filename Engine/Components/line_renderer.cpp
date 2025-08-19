#include "pch.h"
#include "line_renderer.h"
#include "Components/camera_component.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"

namespace engine
{
void LineRenderer::SetVertices(std::vector<Vertex> vertices)
{
    m_vertex_buffer_ = std::make_shared<VertexBuffer>(vertices.size(), vertices.data());
    if (m_vertex_buffer_)
    {
        m_vertex_buffer_ = nullptr;
    }
    m_vertex_buffer_ = std::make_shared<VertexBuffer>(vertices.size(), vertices.data());
    if (!m_vertex_buffer_->IsValid())
    {
        m_is_valid_ = false;
    }
}

void LineRenderer::SetIndices(std::vector<uint32_t> indices)
{
    m_num_indices_ = indices.size();
    if (m_index_buffer_)
    {
        m_index_buffer_ = nullptr;
    }
    m_index_buffer_ = std::make_shared<IndexBuffer>(m_num_indices_ * sizeof(uint32_t), indices.data());
    if (!m_vertex_buffer_->IsValid())
    {
        m_is_valid_ = false;
    }
}

void LineRenderer::OnInspectorGui()
{
    Renderer::OnInspectorGui();
}

void LineRenderer::OnDraw()
{
    const auto camera = Camera::Main();
    const Matrix view = camera->ViewMatrix();
    const Matrix proj = camera->ProjectionMatrix();

    const auto current_buffer_idx = g_RenderEngine->CurrentBackBufferIndex();
    const auto &view_projection_buffer = m_view_projection_buffers_[current_buffer_idx];
    const auto view_projection = view_projection_buffer->GetPtr<Matrix>();
    view_projection[0] = view;
    view_projection[1] = proj;

    const auto cmd_list = g_RenderEngine->CommandList();
    cmd_list->SetPipelineState(PSOManager::Get("Line"));
    cmd_list->SetGraphicsRootConstantBufferView(0, view_projection_buffer->GetAddress());
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
    cmd_list->IASetVertexBuffers(0, 1, m_vertex_buffer_->View());
    cmd_list->IASetIndexBuffer(m_index_buffer_->View());
    cmd_list->DrawIndexedInstanced(m_num_indices_, 1, 0, 0, 0);
}
}