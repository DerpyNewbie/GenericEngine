#include "pch.h"
#include "StructuredBuffer.h"

#include "DirectXResourceFactory.h"
#include "RenderEngine.h"

namespace engine
{
void StructuredBuffer::CreateBuffer()
{
    const auto total_size = m_stride_ * m_element_count_;
    CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Buffer(total_size);
    resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    const auto default_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    m_current_state_ = D3D12_RESOURCE_STATE_COMMON;
    m_default_buffer_ = DirectXResourceFactory::CreateBuffer(default_heap_prop, resource_desc, D3D12_RESOURCE_STATE_COMMON, D3D12_HEAP_FLAG_NONE, nullptr);

    if (m_default_buffer_ == nullptr)
    {
        Logger::Error<StructuredBuffer>("Failed to Create StructuredBuffer Resource (DefaultBuffer)");
        return;
    }
    m_default_buffer_->SetName(L"StructuredBuffer_Default");

    const auto upload_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    m_upload_buffer_ = DirectXResourceFactory::CreateBuffer(upload_heap_prop, resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_FLAG_NONE, nullptr);

    if (m_upload_buffer_ == nullptr)
    {
        Logger::Error<StructuredBuffer>("Failed to Create StructuredBuffer Resource (UploadBuffer)");
        return;
    }
    m_upload_buffer_->SetName(L"StructuredBuffer_Upload");

    m_gpu_address_ = m_default_buffer_->GetGPUVirtualAddress();
}

void StructuredBuffer::UpdateBuffer(const void *data)
{
    if (m_upload_buffer_ == nullptr || m_default_buffer_ == nullptr)
    {
        Logger::Error<StructuredBuffer>("UpdateBuffer failed: StructuredBuffer is not initialized");
        return;
    }

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

    RenderEngine::CommandList()->CopyBufferRegion(
        m_default_buffer_.Get(),
        0,
        m_upload_buffer_.Get(),
        0,
        m_stride_ * m_element_count_
    );
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_default_buffer_.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_COMMON
    );
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);
}

void StructuredBuffer::UploadBuffer(const std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav)
{
    const auto device = RenderEngine::Device();
    if (is_uav)
    {
        const auto uav_desc = UavDesc();
        device->CreateUnorderedAccessView(m_default_buffer_.Get(), nullptr, &uav_desc, desc_handle->handle_cpu);
    }
    else
    {
        const auto view_desc = ViewDesc();
        RenderEngine::Device()->CreateShaderResourceView(m_default_buffer_.Get(), &view_desc, desc_handle->handle_cpu);
    }
}

std::shared_ptr<DescriptorHandle> StructuredBuffer::UploadBuffer()
{
    return DescriptorHeap::Register(this);
}

bool StructuredBuffer::IsValid()
{
    return m_default_buffer_ != nullptr;
}

bool StructuredBuffer::Transition(D3D12_RESOURCE_STATES new_state)
{
    if (m_current_state_ == new_state)
        return false;

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_default_buffer_.Get(), m_current_state_,
        new_state);
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);

    m_current_state_ = new_state;
    return true;
}

D3D12_UNORDERED_ACCESS_VIEW_DESC StructuredBuffer::UavDesc() const
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};

    uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uav_desc.Format = DXGI_FORMAT_UNKNOWN;
    uav_desc.Buffer.FirstElement = 0;
    uav_desc.Buffer.StructureByteStride = m_stride_;
    uav_desc.Buffer.CounterOffsetInBytes = 0;
    uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

    return uav_desc;
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