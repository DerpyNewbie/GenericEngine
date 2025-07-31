#include "pch.h"
#include "line_renderer.h"
#include "Components/camera.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"

namespace engine
{
void LineRenderer::SetVertices(std::vector<Vertex> vertices)
{
    m_vertex_buffer = std::make_shared<VertexBuffer>(vertices.size(), vertices.data());
    if (m_vertex_buffer)
    {
        m_vertex_buffer = nullptr;
    }
    m_vertex_buffer = std::make_shared<VertexBuffer>(vertices.size(), vertices.data());
    if (!m_vertex_buffer->IsValid())
    {
        m_is_valid = false;
    }
}

void LineRenderer::SetIndices(std::vector<uint32_t> indices)
{
    m_num_indices = indices.size();
    if (m_index_buffer)
    {
        m_index_buffer = nullptr;
    }
    m_index_buffer = std::make_shared<IndexBuffer>(m_num_indices * sizeof(uint32_t), indices.data());
    if (!m_vertex_buffer->IsValid())
    {
        m_is_valid = false;
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

    for (auto &vp_buffer : m_view_projection_buffers)
    {
        auto ptr = vp_buffer->GetPtr<Matrix>();
        ptr[0] = view;
        ptr[1] = proj;
    }

    auto cmd_list = g_RenderEngine->CommandList();
    auto currentIndex = g_RenderEngine->CurrentBackBufferIndex();
    auto vbView = m_vertex_buffer->View();
    auto ibView = m_index_buffer->View();

    cmd_list->SetPipelineState(PSOManager::Get("Line"));
    cmd_list->SetGraphicsRootConstantBufferView(0, m_view_projection_buffers[currentIndex]->GetAddress());
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
    cmd_list->IASetVertexBuffers(0, 1, &vbView);
    cmd_list->IASetIndexBuffer(&ibView);
    cmd_list->DrawIndexedInstanced(m_num_indices, 1, 0, 0, 0);
}
}