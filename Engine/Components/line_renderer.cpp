#include "pch.h"
#include "line_renderer.h"
#include "Components/camera.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"


void engine::LineRenderer::SetVertices(std::vector<Vertex> vertices)
{
    m_VertexBuffer = std::make_shared<engine::VertexBuffer>(vertices.size(), vertices.data());
    if (m_VertexBuffer)
    {
        m_VertexBuffer = nullptr;
    }
    m_VertexBuffer = std::make_shared<engine::VertexBuffer>(vertices.size(), vertices.data());
    if (!m_VertexBuffer->IsValid())
    {
        m_IsValid = false;
    }
}

void engine::LineRenderer::SetIndices(std::vector<uint32_t> indices)
{
    m_NumIndices = indices.size();
    if (m_IndexBuffer)
    {
        m_IndexBuffer = nullptr;
    }
    m_IndexBuffer = std::make_shared<engine::IndexBuffer>(m_NumIndices * sizeof(uint32_t), indices.data());
    if (!m_VertexBuffer->IsValid())
    {
        m_IsValid = false;
    }
}

void engine::LineRenderer::OnInspectorGui()
{
    Renderer::OnInspectorGui();
}

void engine::LineRenderer::OnDraw()
{
    const auto camera = Camera::Main();
    const Matrix view = camera->GetViewMatrix();
    const Matrix proj = camera->GetProjectionMatrix();

    for (auto &vp_buffer : m_ViewProjectionBuffers)
    {
        auto ptr = vp_buffer->GetPtr<Matrix>();
        ptr[0] = view;
        ptr[1] = proj;
    }

    auto cmd_list = g_RenderEngine->CommandList();
    auto currentIndex = g_RenderEngine->CurrentBackBufferIndex();
    auto vbView = m_VertexBuffer->View();
    auto ibView = m_IndexBuffer->View();

    cmd_list->SetPipelineState(PSOManager::Get("Line"));
    cmd_list->SetGraphicsRootConstantBufferView(0, m_ViewProjectionBuffers[currentIndex]->GetAddress());
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
    cmd_list->IASetVertexBuffers(0, 1, &vbView);
    cmd_list->IASetIndexBuffer(&ibView);
    cmd_list->DrawIndexedInstanced(m_NumIndices, 1, 0, 0, 0);
}