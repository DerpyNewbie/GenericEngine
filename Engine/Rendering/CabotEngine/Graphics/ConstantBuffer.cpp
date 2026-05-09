#include "pch.h"
#include "ConstantBuffer.h"
#include "DescriptorHeap.h"
#include "DirectXResourceFactory.h"
#include "RenderEngine.h"

ConstantBuffer::ConstantBuffer(const size_t size)
{
    m_size_ = size;

    constexpr size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
    m_size_aligned_ = ((size + (align - 1)) & ~(align - 1));
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetAddress() const
{
    return m_desc_.BufferLocation;
}

D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBuffer::ViewDesc() const
{
    return m_desc_;
}

bool ConstantBuffer::CreateBuffer()
{
    const auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto desc = CD3DX12_RESOURCE_DESC::Buffer(m_size_aligned_);

    m_buffer_ = DirectXResourceFactory::CreateBuffer(
        prop,
        desc,
        D3D12_RESOURCE_STATE_GENERIC_READ);

    if (m_buffer_ == nullptr)
    {
        engine::Logger::Error<ConstantBuffer>("failed to create constant buffer resource");
        return false;
    }

    constexpr D3D12_RANGE unreadable_range = {0, 0};
    const auto hr = m_buffer_->Map(0, &unreadable_range, &m_p_mapped_ptr_);
    if (FAILED(hr))
    {
        engine::Logger::Error<ConstantBuffer>("failed to constant buffer mapping");
        return false;
    }

    m_desc_ = {};
    m_desc_.BufferLocation = m_buffer_->GetGPUVirtualAddress();
    m_desc_.SizeInBytes = static_cast<UINT>(m_size_aligned_);

    m_buffer_->SetName(L"ConstantBuffer");

    return true;
}

void ConstantBuffer::UpdateBuffer(const void *data) const
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
    return m_buffer_ != nullptr;
}

bool ConstantBuffer::CanUpdate()
{
    return true;
}

void *ConstantBuffer::GetPtr() const
{
    return m_p_mapped_ptr_;
}