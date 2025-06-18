#include "pch.h"

#include "VertexBuffer.h"
#include "RenderEngine.h"
#include "Rendering/Vertex.h"
#include "Components/skinned_mesh_renderer.h"

#include <directx/d3dx12.h>

engine::VertexBuffer::VertexBuffer(const Mesh *pInitData)
{
    auto size = sizeof(Vertex) * pInitData->vertices.size();
    auto stride = sizeof(Vertex);

    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // ヒーププロパティ
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(size); // リソースの設定

    // リソースを生成
    auto hr = g_RenderEngine->Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_pBuffer.GetAddressOf()));

    if (FAILED(hr))
    {
        printf("頂点バッファリソースの生成に失敗");
        return;
    }

    // 頂点バッファビューの設定
    m_View.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
    m_View.SizeInBytes = static_cast<UINT>(size);
    m_View.StrideInBytes = static_cast<UINT>(stride);

    // マッピングする
    if (pInitData != nullptr)
    {
        void *ptr = nullptr;
        hr = m_pBuffer->Map(0, nullptr, &ptr);
        if (FAILED(hr))
        {
            printf("頂点バッファマッピングに失敗");
            return;
        }

        std::vector<Vertex> ptr_;
        ptr_.resize(pInitData->vertices.size());
        for (size_t i = 0; i < pInitData->vertices.size(); i++)
        {
            ptr_.at(i).vertex = pInitData->vertices[i];
        }
        for (size_t i = 0; i < pInitData->colors.size(); i++)
        {
            ptr_.at(i).color = pInitData->colors[i];
        }
        for (size_t i = 0; i < pInitData->normals.size(); i++)
        {
            ptr_.at(i).normal = pInitData->normals[i];
        }
        for (size_t i = 0; i < pInitData->tangents.size(); i++)
        {
            ptr_.at(i).tangent = pInitData->tangents[i];
        }
        for (size_t i = 0; i < pInitData->uvs.size(); i++)
        {
            for (size_t j = 0; j < pInitData->uvs[i].size(); j++)
            {
                ptr_.at(i).uvs[j] = pInitData->uvs[i][j];
            }
        }
        for (size_t i = 0; i < pInitData->bone_weights.size(); i++)
        {
            for (unsigned char j = 0; j < SkinnedMeshRenderer::kLimitBonesPerVertex; j++)
            {
                ptr_.at(i).bone_index[j] = pInitData->bone_weights[i][j].bone_index;
                ptr_.at(i).bone_weight[j] = pInitData->bone_weights[i][j].weight;
            }
        }
        memcpy(ptr, ptr_.data(), size);
        // マッピング解除
        m_pBuffer->Unmap(0, nullptr);
    }

    m_IsValid = true;
}

D3D12_VERTEX_BUFFER_VIEW engine::VertexBuffer::View() const
{
    return m_View;
}

bool engine::VertexBuffer::IsValid()
{
    return m_IsValid;
}