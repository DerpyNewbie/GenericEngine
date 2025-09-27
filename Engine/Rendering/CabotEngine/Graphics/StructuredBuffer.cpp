#include "pch.h"
#include "StructuredBuffer.h"

#include "RenderEngine.h"

namespace engine
{
void StructuredBuffer::CreateBuffer()
{
    m_is_valid_ = false;

    const auto total_size = m_stride_ * m_element_count_;
    const CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Buffer(total_size);

    // create a default heap (used for gpu data processing)
    const auto default_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &default_heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &resource_desc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&m_default_buffer_)
        );

    if (FAILED(hr))
    {
        Logger::Error<StructuredBuffer>("Failed to Create StructuredBuffer Resource (DefaultBuffer)");
        return;
    }

    hr = m_default_buffer_->SetName(L"StructuredBuffer_Default");
    if (FAILED(hr))
    {
        Logger::Error<StructuredBuffer>("Failed to SetName StructuredBuffer Resource (DefaultBuffer)");
        return;
    }

    // create an upload heap (used for updating the default buffer with new data)
    const auto upload_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    hr = RenderEngine::Device()->CreateCommittedResource(
        &upload_heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &resource_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_upload_buffer_)
        );

    if (FAILED(hr))
    {
        Logger::Error<StructuredBuffer>("Failed to Create StructuredBuffer Resource (UploadBuffer)");
        return;
    }

    hr = m_upload_buffer_->SetName(L"StructuredBuffer_Upload");

    if (FAILED(hr))
    {
        Logger::Error<StructuredBuffer>("Failed to SetName StructuredBuffer Resource (UploadBuffer)");
        return;
    }

    m_gpu_address_ = m_upload_buffer_->GetGPUVirtualAddress();
    m_is_valid_ = true;
}

void StructuredBuffer::UpdateBuffer(void *data)
{
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_default_buffer_.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_COPY_DEST
        );
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);

    void *mapped = nullptr;
    m_upload_buffer_->Map(0, nullptr, &mapped);
    memcpy(mapped, data, m_stride_ * m_element_count_);
    m_upload_buffer_->Unmap(0, nullptr);

    RenderEngine::CommandList()->CopyBufferRegion(m_default_buffer_.Get(), 0, m_upload_buffer_.Get(), 0,
                                                  m_element_count_);
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_default_buffer_.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_COMMON
        );
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);
}

std::shared_ptr<DescriptorHandle> StructuredBuffer::DescriptorHandle()
{
    return DescriptorHeap::Register(this);
}

bool StructuredBuffer::IsValid()
{
    return m_is_valid_;
}

D3D12_SHADER_RESOURCE_VIEW_DESC StructuredBuffer::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = static_cast<UINT>(m_element_count_);
    srvDesc.Buffer.StructureByteStride = static_cast<UINT>(m_stride_);
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    return srvDesc;
}

ID3D12Resource *StructuredBuffer::Resource()
{
    return m_default_buffer_.Get();
}

D3D12_GPU_VIRTUAL_ADDRESS StructuredBuffer::GetAddress() const
{
    return m_gpu_address_;
}
}