#include "pch.h"
#include "StructuredBuffer.h"

#include "DirectXResourceFactory.h"
#include "RenderEngine.h"

namespace engine
{
void StructuredBuffer::CreateReadBackResource()
{
    const auto total_size = m_stride_ * m_element_count_;
    CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Buffer(total_size);
    resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    const auto default_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);

    m_current_state_ = D3D12_RESOURCE_STATE_COMMON;
    m_readback_resource_ = DirectXResourceFactory::CreateBuffer(default_heap_prop, resource_desc, D3D12_RESOURCE_STATE_COMMON, D3D12_HEAP_FLAG_NONE, nullptr);

    if (m_readback_resource_ == nullptr)
    {
        Logger::Error<StructuredBuffer>("Failed to Create StructuredBuffer Resource");
        return;
    }
    m_readback_resource_->SetName(L"StructuredBuffer_Default");
}

StructuredBuffer::~StructuredBuffer()
{
    DirectXResourceFactory::ReleaseResource(m_upload_resource_);
    DirectXResourceFactory::ReleaseResource(m_default_resource_);
}

void StructuredBuffer::CreateBuffer()
{
    const auto total_size = m_stride_ * m_element_count_;
    CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Buffer(total_size);
    resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    const auto default_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    m_current_state_ = D3D12_RESOURCE_STATE_COMMON;
    m_default_resource_ = DirectXResourceFactory::CreateBuffer(default_heap_prop, resource_desc, D3D12_RESOURCE_STATE_COMMON, D3D12_HEAP_FLAG_NONE, nullptr);

    if (m_default_resource_ == nullptr)
    {
        Logger::Error<StructuredBuffer>("Failed to Create StructuredBuffer Resource (DefaultBuffer)");
        return;
    }
    m_default_resource_->SetName(L"StructuredBuffer_Default");

    const auto upload_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    m_upload_resource_ = DirectXResourceFactory::CreateBuffer(upload_heap_prop, resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_HEAP_FLAG_NONE, nullptr);

    if (m_upload_resource_ == nullptr)
    {
        Logger::Error<StructuredBuffer>("Failed to Create StructuredBuffer Resource (UploadBuffer)");
        return;
    }
    m_upload_resource_->SetName(L"StructuredBuffer_Upload");

    m_gpu_address_ = m_default_resource_->GetGPUVirtualAddress();
}

void StructuredBuffer::UpdateBuffer(const void *data)
{
    if (m_upload_resource_ == nullptr || m_default_resource_ == nullptr)
    {
        Logger::Error<StructuredBuffer>("UpdateBuffer failed: StructuredBuffer is not initialized");
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
    memcpy(mapped, data, m_stride_ * m_element_count_);
    m_upload_resource_->Unmap(0, nullptr);

    RenderEngine::CommandList()->CopyBufferRegion(
        m_default_resource_.Get(),
        0,
        m_upload_resource_.Get(),
        0,
        m_stride_ * m_element_count_
    );
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_default_resource_.Get(),
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
        device->CreateUnorderedAccessView(m_default_resource_.Get(), nullptr, &uav_desc, desc_handle->handle_cpu);
    }
    else
    {
        const auto view_desc = ViewDesc();
        RenderEngine::Device()->CreateShaderResourceView(m_default_resource_.Get(), &view_desc, desc_handle->handle_cpu);
    }
}

bool StructuredBuffer::IsValid()
{
    return m_default_resource_ != nullptr;
}

bool StructuredBuffer::Transition(D3D12_RESOURCE_STATES new_state)
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

void StructuredBuffer::RequestReadBack()
{
    if (m_readback_resource_ == nullptr)
        CreateReadBackResource();

    auto cmd_list = RenderEngine::CommandList();

    Transition(D3D12_RESOURCE_STATE_COPY_DEST);

    cmd_list->CopyBufferRegion(
        m_readback_resource_.Get(),
        0,
        m_default_resource_.Get(),
        0,
        m_stride_ * m_element_count_);

    m_read_back_fence_value_ = RenderEngine::FenceNextValue();
}

bool StructuredBuffer::FetchBufferData(void *data)
{
    if (m_readback_resource_ == nullptr || data == nullptr)
    {
        Logger::Error<StructuredBuffer>("FetchBufferData failed: Readback resource is not initialized or output pointer is null");
        return false;
    }

    if (!RenderEngine::IsFenceComplete(m_read_back_fence_value_))
        return false;

    D3D12_RANGE readback_range = {0, m_stride_ * m_element_count_};
    D3D12_RANGE write_range = {0, 0};

    void *mapped = nullptr;
    m_readback_resource_->Map(0, &readback_range, &mapped);
    memcpy(data, mapped, m_stride_ * m_element_count_);
    m_readback_resource_->Unmap(0, &write_range);
    return true;
}

D3D12_UNORDERED_ACCESS_VIEW_DESC StructuredBuffer::UavDesc() const
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};

    uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uav_desc.Format = DXGI_FORMAT_UNKNOWN;
    uav_desc.Buffer.FirstElement = 0;
    uav_desc.Buffer.StructureByteStride = m_stride_;
    uav_desc.Buffer.NumElements = m_element_count_;
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
    return m_default_resource_.Get();
}

D3D12_GPU_VIRTUAL_ADDRESS StructuredBuffer::GetAddress() const
{
    return m_gpu_address_;
}
}