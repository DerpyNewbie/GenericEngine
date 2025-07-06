#include "pch.h"
#include "line_renderer.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "Components/camera.h"
#include "Components/mesh_renderer.h"

void LineRenderer::UpdateVertexBuffer(std::vector<Vertex> vertices)
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

void LineRenderer::UpdateIndexBuffer(std::vector<uint32_t> indices)
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

void LineRenderer::UpdateViewProjectionBuffer()
{
    const auto camera = engine::Camera::Main();
    const Matrix view = camera.lock()->GetViewMatrix();
    const Matrix proj = camera.lock()->GetProjectionMatrix();

    for (auto &vp_buffer : m_ViewProjectionBuffers)
    {
        auto ptr = vp_buffer->GetPtr<Matrix>();
        ptr[0] = view;
        ptr[1] = proj;
    }
}

LineRenderer::LineRenderer(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    UpdateVertexBuffer(vertices);
    UpdateIndexBuffer(indices);
    for (auto &vp_buffer : m_ViewProjectionBuffers)
    {
        vp_buffer = std::make_shared<ConstantBuffer>(sizeof(Matrix) * 2);
        if (!vp_buffer->IsValid())
        {
            engine::Logger::Error<LineRenderer>("Failed to create ViewProjection buffer!");
        }
    }
}

void LineRenderer::Draw()
{
    UpdateViewProjectionBuffer();

    auto cmd_list = g_RenderEngine->CommandList();
    auto currentIndex = g_RenderEngine->CurrentBackBufferIndex();
    auto vbView = m_VertexBuffer->View();
    auto ibView = m_IndexBuffer->View();

    cmd_list->SetPipelineState(g_PSOManager.Get("Line"));
    cmd_list->SetGraphicsRootConstantBufferView(0, m_ViewProjectionBuffers[currentIndex]->GetAddress());
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
    cmd_list->IASetVertexBuffers(0, 1, &vbView);
    cmd_list->IASetIndexBuffer(&ibView);
    cmd_list->DrawIndexedInstanced(m_NumIndices, 1, 0, 0, 0);
}