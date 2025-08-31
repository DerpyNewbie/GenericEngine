#include "pch.h"

#include "VertexBuffer.h"
#include "RenderEngine.h"
#include "Rendering/Vertex.h"
#include "Components/skinned_mesh_renderer.h"

namespace engine
{
VertexBuffer::VertexBuffer(const Mesh *p_init_data)
{
    auto size = sizeof(Vertex) * p_init_data->vertices.size();
    auto stride = sizeof(Vertex);

    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // ヒーププロパティ
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(size); // リソースの設定

    // リソースを生成
    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_pBuffer.GetAddressOf()));

    if (FAILED(hr))
    {
        Logger::Error<VertexBuffer>("Failed to Create VertexBuffer Resource");
        return;
    }

    // 頂点バッファビューの設定
    m_View.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
    m_View.SizeInBytes = static_cast<UINT>(size);
    m_View.StrideInBytes = static_cast<UINT>(stride);

    // マッピングする
    if (p_init_data != nullptr)
    {
        void *ptr = nullptr;
        hr = m_pBuffer->Map(0, nullptr, &ptr);
        if (FAILED(hr))
        {
            Logger::Error<VertexBuffer>("Failed to mapping VertexBuffer");
            return;
        }

        std::vector<Vertex> ptr_;
        ptr_.resize(p_init_data->vertices.size());
        for (size_t i = 0; i < p_init_data->vertices.size(); ++i)
        {
            ptr_.at(i).vertex = p_init_data->vertices[i];
        }
        for (size_t i = 0; i < p_init_data->colors.size(); ++i)
        {
            ptr_.at(i).color = p_init_data->colors[i];
        }
        for (size_t i = 0; i < p_init_data->normals.size(); ++i)
        {
            ptr_.at(i).normal = p_init_data->normals[i];
        }
        for (size_t i = 0; i < p_init_data->tangents.size(); ++i)
        {
            ptr_.at(i).tangent = p_init_data->tangents[i];
        }
        for (size_t i = 0; i < p_init_data->uvs.size(); ++i)
        {
            for (size_t j = 0; j < p_init_data->uvs[i].size(); ++j)
            {
                ptr_.at(j).uvs[i] = p_init_data->uvs[i][j];
            }
        }
        for (size_t i = 0; i < p_init_data->bone_weights.size(); ++i)
        {
            ptr_.at(i).bones_per_vertex = static_cast<unsigned int>(p_init_data->bone_weights[i].size());
            for (size_t j = 0; j < p_init_data->bone_weights[i].size(); ++j)
            {
                ptr_.at(i).bone_index[j] = p_init_data->bone_weights[i][j].bone_index;
                ptr_.at(i).bone_weight[j] = p_init_data->bone_weights[i][j].weight;
            }
        }
        memcpy(ptr, ptr_.data(), size);
        // マッピング解除
        m_pBuffer->Unmap(0, nullptr);
    }

    m_IsValid = true;
}

VertexBuffer::VertexBuffer(size_t num_vertices, const Vertex *p_init_data)
{
    auto size = sizeof(Vertex) * num_vertices;
    auto stride = sizeof(Vertex);

    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // ヒーププロパティ
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(size); // リソースの設定

    // リソースを生成
    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_pBuffer.GetAddressOf()));

    if (FAILED(hr))
    {
        Logger::Error<VertexBuffer>("Failed to Create VertexBuffer Resource");
        return;
    }

    // 頂点バッファビューの設定
    m_View.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
    m_View.SizeInBytes = static_cast<UINT>(size);
    m_View.StrideInBytes = static_cast<UINT>(stride);

    // マッピングする
    if (p_init_data != nullptr)
    {
        void *ptr = nullptr;
        hr = m_pBuffer->Map(0, nullptr, &ptr);
        if (FAILED(hr))
        {
            Logger::Error<VertexBuffer>("Failed to mapping VertexBuffer");
            return;
        }

        memcpy(ptr, p_init_data, size);
        // マッピング解除
        m_pBuffer->Unmap(0, nullptr);
    }

    m_IsValid = true;
}

D3D12_VERTEX_BUFFER_VIEW *VertexBuffer::View()
{
    return &m_View;
}

bool VertexBuffer::IsValid()
{
    return m_IsValid;
}
}