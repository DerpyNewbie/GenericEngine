#include "pch.h"
#include "StructuredBuffer.h"

void engine::StructuredBuffer::CreateBuffer()
{
    size_t totalSize = m_stride * m_elementCount;

    // GPU側リソース
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);

    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    g_RenderEngine->Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&m_pDefaultBuffer)
        );

    // アップロード用バッファ
    auto hr = g_RenderEngine->Device()->CreateCommittedResource(
        &heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_pUploadBuffer)
        );

    if (FAILED(hr))
    {
        return;
    }
    m_IsValid = true;
}

void engine::StructuredBuffer::UpdateBuffer(void *data)
{
    void *mapped = nullptr;
    m_pUploadBuffer->Map(0, nullptr, &mapped);
    memcpy(mapped, data, m_stride * m_elementCount);
    m_pUploadBuffer->Unmap(0, nullptr);

    g_RenderEngine->CommandList()->CopyBufferRegion(m_pDefaultBuffer.Get(), 0, m_pUploadBuffer.Get(), 0,
                                                    m_elementCount);
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pDefaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
        );
    g_RenderEngine->CommandList()->ResourceBarrier(1, &barrier);
}

std::shared_ptr<DescriptorHandle> engine::StructuredBuffer::UploadBuffer()
{
    auto pHandle = g_DescriptorHeapManager->Get().Register(*this);
    return pHandle;
}

D3D12_SHADER_RESOURCE_VIEW_DESC engine::StructuredBuffer::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = m_elementCount;
    srvDesc.Buffer.StructureByteStride = m_stride;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    return srvDesc;
}

ID3D12Resource *engine::StructuredBuffer::Resource()
{
    return m_pUploadBuffer.Get();
}

bool engine::StructuredBuffer::IsValid()
{
    return m_IsValid;
}