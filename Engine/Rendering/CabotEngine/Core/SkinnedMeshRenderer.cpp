#include "pch.h"

#include "SkinnedMeshRenderer.h"
#include "../Graphics/DescriptorHeapManager.h"
#include "../Graphics/PSOManager.h"

void SkinnedMeshRenderer::LoadFromFBX(std::wstring file_path)
{
    /*m_Meshes.clear();

    for (auto& mesh : m_Meshes)
    {
        auto size = sizeof(Vertex) * mesh.Vertices.size();
        auto stride = sizeof(Vertex);
        auto pVB = std::make_shared<VertexBuffer>(size, stride, mesh.Vertices.data());

        m_VertexBuffers.emplace_back(pVB);

        size = sizeof(uint32_t) * mesh.Indices.size();
        auto indice = mesh.Indices.data();
        auto pIB = std::make_shared<IndexBuffer>(size, indice);

        m_IndexBuffers.emplace_back(pIB);
    }*/
}

void SkinnedMeshRenderer::Draw()
{
    for (size_t i = 0; i < m_Meshes.size(); i++)
    {
        auto currentIndex = g_RenderEngine->CurrentBackBufferIndex();
        auto cmd_list = g_RenderEngine->CommandList();

        cmd_list->SetPipelineState(g_PSOManager.Get("Basic"));
        cmd_list->SetGraphicsRootConstantBufferView(0, m_WVPBuffer[currentIndex].GetAddress());

        auto vbView = m_VertexBuffers[i]->View();
        auto ibView = m_IndexBuffers[i]->View();

        cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmd_list->IASetVertexBuffers(0, 1, &vbView);
        cmd_list->IASetIndexBuffer(&ibView);

        cmd_list->SetGraphicsRootDescriptorTable(6, m_MaterialHandle[0].HandleGPU); // そのメッシュに対応するディスクリプタテーブルをセット

        cmd_list->DrawIndexedInstanced(m_Meshes[i].Indices.size(), 1, 0, 0, 0);
    }
}