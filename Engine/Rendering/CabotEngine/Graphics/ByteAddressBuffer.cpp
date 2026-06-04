#include "pch.h"
#include "ByteAddressBuffer.h"
#include "DescriptorHeap.h"
#include "DirectXResourceFactory.h"

namespace engine
{
ByteAddressBuffer::ByteAddressBuffer(const size_t elem_count) : m_element_count_(elem_count)
{}

void ByteAddressBuffer::CreateBuffer()
{
    D3D12_RESOURCE_DESC resource_desc;
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Width = m_element_count_;
    resource_desc.Height = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.MipLevels = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    const auto default_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    m_current_state_ = D3D12_RESOURCE_STATE_COMMON;
    m_default_resource_ = DirectXResourceFactory::CreateBuffer(default_heap_prop, resource_desc, D3D12_RESOURCE_STATE_COMMON, D3D12_HEAP_FLAG_NONE, nullptr);

    if (m_default_resource_ == nullptr)
    {
        Logger::Error<ByteAddressBuffer>("Failed to create ByteAddressBuffer");
        return;
    }
    m_default_resource_->SetName(L"ByteAddressBufferDefault");

    const auto upload_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    m_upload_resource_ = DirectXResourceFactory::CreateBuffer(upload_heap_prop, resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_FLAG_NONE, nullptr);

    if (m_upload_resource_ == nullptr)
    {
        Logger::Error<ByteAddressBuffer>("Failed to create ByteAddressBuffer");
        return;
    }
    m_upload_resource_->SetName(L"ByteAddressBufferUpload");

    m_gpu_address_ = m_default_resource_->GetGPUVirtualAddress();
}

void ByteAddressBuffer::UpdateBuffer(const void *data)
{
    if (m_upload_resource_ == nullptr || m_default_resource_ == nullptr)
    {
        Logger::Error<ByteAddressBuffer>("UpdateBuffer failed: ByteAddressBuffer is not initialized");
        return;
    }

    if (data == nullptr)
        return;

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_default_resource_.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_COPY_DEST
    );
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);

    void *mapped = nullptr;
    m_upload_resource_->Map(0, nullptr, &mapped);
    memcpy(mapped, data, m_element_count_ * sizeof(uint8_t));
    m_upload_resource_->Unmap(0, nullptr);

    RenderEngine::CommandList()->CopyBufferRegion(
        m_default_resource_.Get(),
        0,
        m_upload_resource_.Get(),
        0,
        m_element_count_ * sizeof(uint8_t)
    );
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_default_resource_.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_COMMON
    );
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);
}

void ByteAddressBuffer::UploadBuffer(const std::shared_ptr<DescriptorHandle> desc_handle, const bool is_uav)
{
    const auto device = RenderEngine::Device();
    if (is_uav)
    {
        const auto uav_desc = UavDesc();
        device->CreateUnorderedAccessView(m_default_resource_.Get(), nullptr, &uav_desc, desc_handle->handle_cpu);
    }
    else
    {
        const auto view_desc = ViewDesc();
        device->CreateShaderResourceView(m_default_resource_.Get(), &view_desc, desc_handle->handle_cpu);
    }
}

std::shared_ptr<DescriptorHandle> ByteAddressBuffer::UploadBuffer()
{
    return DescriptorHeap::Register(this);
}

bool ByteAddressBuffer::IsValid()
{
    return m_default_resource_ != nullptr;
}

bool ByteAddressBuffer::Transition(D3D12_RESOURCE_STATES new_state)
{
    if (m_current_state_ == new_state)
        return false;

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_default_resource_.Get(), m_current_state_,
        new_state);
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);

    m_current_state_ = new_state;
    return true;
}

D3D12_UNORDERED_ACCESS_VIEW_DESC ByteAddressBuffer::UavDesc() const
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};

    uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uav_desc.Format = DXGI_FORMAT_R32_TYPELESS;
    uav_desc.Buffer.FirstElement = 0;
    uav_desc.Buffer.StructureByteStride = 0;
    uav_desc.Buffer.NumElements = static_cast<UINT>(m_element_count_);
    uav_desc.Buffer.CounterOffsetInBytes = 0;
    uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

    return uav_desc;
}

D3D12_SHADER_RESOURCE_VIEW_DESC ByteAddressBuffer::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.StructureByteStride = 0;
    srvDesc.Buffer.NumElements = static_cast<UINT>(m_element_count_);
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    return srvDesc;
}

ID3D12Resource *ByteAddressBuffer::Resource()
{
    return m_default_resource_.Get();
}

D3D12_GPU_VIRTUAL_ADDRESS ByteAddressBuffer::GetAddress() const
{
    return m_gpu_address_;
}
}