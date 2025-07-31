#include "pch.h"
#include "line_renderer.h"
#include "Components/camera.h"
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
    const Matrix view = camera->GetViewMatrix();
    const Matrix proj = camera->GetProjectionMatrix();

    for (auto &vp_buffer : m_view_projection_buffers_)
    {
        auto ptr = vp_buffer->GetPtr<Matrix>();
        ptr[0] = view;
        ptr[1] = proj;
    }

    auto cmd_list = g_RenderEngine->CommandList();
    auto currentIndex = g_RenderEngine->CurrentBackBufferIndex();
    auto vbView = m_vertex_buffer_->View();
    auto ibView = m_index_buffer_->View();

    cmd_list->SetPipelineState(PSOManager::Get("Line"));
    cmd_list->SetGraphicsRootConstantBufferView(0, m_view_projection_buffers_[currentIndex]->GetAddress());
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
    cmd_list->IASetVertexBuffers(0, 1, &vbView);
    cmd_list->IASetIndexBuffer(&ibView);
    cmd_list->DrawIndexedInstanced(m_num_indices_, 1, 0, 0, 0);
}
}