#include "pch.h"
#include "byte_address_buffer.h"
#include "DescriptorHeap.h"
#include "RenderEngine.h"

engine::ByteAddressBuffer::ByteAddressBuffer(ID3D12Resource *resource)
{
    m_size_ = resource->GetDesc().Width;
    if (m_size_ & 3 == 0) // リソースのサイズが4の倍数じゃなかったら無理
    {
        m_is_valid_ = false;
        m_default_buffer_ = nullptr;
        return;
    }

    m_can_update_ = false;
    m_default_buffer_ = resource;

    m_gpu_address_ = 0;
}

void engine::ByteAddressBuffer::CreateBuffer()
{
    m_is_valid_ = false;

    const CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Buffer(m_size_);

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
        Logger::Error<ByteAddressBuffer>("Failed to Create StructuredBuffer Resource (DefaultBuffer)");
        return;
    }

    hr = m_default_buffer_->SetName(L"StructuredBuffer_Default");
    if (FAILED(hr))
    {
        Logger::Error<ByteAddressBuffer>("Failed to SetName StructuredBuffer Resource (DefaultBuffer)");
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
        Logger::Error<ByteAddressBuffer>("Failed to Create StructuredBuffer Resource (UploadBuffer)");
        return;
    }

    hr = m_upload_buffer_->SetName(L"StructuredBuffer_Upload");

    if (FAILED(hr))
    {
        Logger::Error<ByteAddressBuffer>("Failed to SetName StructuredBuffer Resource (UploadBuffer)");
        return;
    }

    m_gpu_address_ = m_default_buffer_->GetGPUVirtualAddress();
    m_is_valid_ = true;
}

void engine::ByteAddressBuffer::UpdateBuffer(void *data)
{
    if (m_upload_buffer_ == nullptr || m_default_buffer_ == nullptr)
    {
        Logger::Error<ByteAddressBuffer>("UpdateBuffer failed: StructuredBuffer is not initialized");
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
    memcpy(mapped, data, m_size_);
    m_upload_buffer_->Unmap(0, nullptr);

    RenderEngine::CommandList()->CopyBufferRegion(
        m_default_buffer_.Get(),
        0,
        m_upload_buffer_.Get(),
        0,
        m_size_
    );
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_default_buffer_.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_COMMON
    );
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);
}

void engine::ByteAddressBuffer::UploadBuffer(const DescriptorHandle &descriptor_handle)
{
    const auto device = RenderEngine::Device();
    const auto resource = Resource();
    const auto desc = ViewDesc();
    device->CreateShaderResourceView(resource, &desc, descriptor_handle.HandleCPU);
}

bool engine::ByteAddressBuffer::IsValid()
{
    return m_is_valid_;
}

D3D12_SHADER_RESOURCE_VIEW_DESC engine::ByteAddressBuffer::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = m_size_ / 4;
    srvDesc.Buffer.StructureByteStride = 0;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
    return srvDesc;
}

ID3D12Resource *engine::ByteAddressBuffer::Resource()
{
    return m_default_buffer_.Get();
}

D3D12_GPU_VIRTUAL_ADDRESS engine::ByteAddressBuffer::GetAddress() const
{
    return m_gpu_address_;
}