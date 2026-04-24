#include "pch.h"
#include "constant_buffer.h"
#include "DescriptorHeap.h"
#include "RenderEngine.h"

ConstantBuffer::ConstantBuffer(const size_t size)
{
    m_size_ = size;

    constexpr size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
    m_size_aligned_ = size + (align - 1) & ~(align - 1);
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetAddress() const
{
    return m_desc_.BufferLocation;
}

D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBuffer::ViewDesc() const
{
    return m_desc_;
}

void ConstantBuffer::CreateBuffer()
{
    const auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto desc = CD3DX12_RESOURCE_DESC::Buffer(m_size_aligned_);

    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_p_buffer_.GetAddressOf()));

    if (FAILED(hr))
    {
        engine::Logger::Error<ConstantBuffer>("failed to create constant buffer resource");
        return;
    }

    hr = m_p_buffer_->Map(0, nullptr, &m_p_mapped_ptr_);
    if (FAILED(hr))
    {
        engine::Logger::Error<ConstantBuffer>("failed to constant buffer mapping");
        return;
    }

    m_desc_ = {};
    m_desc_.BufferLocation = m_p_buffer_->GetGPUVirtualAddress();
    m_desc_.SizeInBytes = static_cast<UINT>(m_size_aligned_);

    m_p_buffer_->SetName(L"ConstantBuffer");
}

void ConstantBuffer::UpdateBuffer(void *data)
{
    memcpy(m_p_mapped_ptr_, data, m_size_);
}

std::shared_ptr<DescriptorHandle> ConstantBuffer::UploadBuffer()
{
    auto pHandle = DescriptorHeap::Register(*this);
    return pHandle;
}

bool ConstantBuffer::IsValid()
{
    return m_p_buffer_ != nullptr;
}

bool ConstantBuffer::CanUpdate()
{
    return true;
}

void *ConstantBuffer::GetPtr() const
{
    return m_p_mapped_ptr_;
}