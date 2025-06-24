#pragma once
#include <d3d12.h>
#include <vector>
#include "ComPtr.h"
#include "RenderEngine.h"


template <typename T>
class StructuredBuffer
{
public:
    StructuredBuffer() = default;

    void Initialize(size_t elementCount)
    {
        m_elementCount = elementCount;
        size_t totalSize = sizeof(T) * elementCount;

        // GPU側リソース
        CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);

        auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

        g_RenderEngine->Device()->CreateCommittedResource(
            &prop,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_COPY_DEST, // 初期状態はコピー先
            nullptr,
            IID_PPV_ARGS(&m_pDefaultBuffer)
            );

        // アップロード用バッファ
        g_RenderEngine->Device()->CreateCommittedResource(
            &heap_prop,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_pUploadBuffer)
            );
    }

    void Upload(const std::vector<T> &data)
    {
        void *mapped = nullptr;
        m_pUploadBuffer->Map(0, nullptr, &mapped);
        memcpy(mapped, data.data(), sizeof(T) * data.size());
        m_pUploadBuffer->Unmap(0, nullptr);

        g_RenderEngine->CommandList()->CopyBufferRegion(m_pDefaultBuffer.Get(), 0, m_pUploadBuffer.Get(), 0,
                                                        m_elementCount);
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_pDefaultBuffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE // StructuredBufferに適した状態
            );
        g_RenderEngine->CommandList()->ResourceBarrier(1, &barrier);
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc()
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = m_elementCount;
        srvDesc.Buffer.StructureByteStride = sizeof(T);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        return srvDesc;
    }

    ID3D12Resource *Resource()
    {
        return m_pDefaultBuffer.Get();
    }

private:
    ComPtr<ID3D12Resource> m_pDefaultBuffer;
    ComPtr<ID3D12Resource> m_pUploadBuffer;
    size_t m_elementCount = 0;
};