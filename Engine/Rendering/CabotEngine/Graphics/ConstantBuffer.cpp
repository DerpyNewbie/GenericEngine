#include "pch.h"
#include "ConstantBuffer.h"
#include "DescriptorHeap.h"
#include "DirectXResourceFactory.h"
#include "RenderEngine.h"

namespace engine
{
ConstantBuffer::ConstantBuffer(const size_t size)
{
    m_size_ = size;

    constexpr size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
    m_size_aligned_ = ((size + (align - 1)) & ~(align - 1));
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetAddress() const
{
    const auto current_back_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    return m_desc_[current_back_buffer_idx].BufferLocation;
}

D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBuffer::ViewDesc() const
{
    const auto current_back_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    return m_desc_[current_back_buffer_idx];
}

void ConstantBuffer::CreateBuffer()
{
    const auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto desc = CD3DX12_RESOURCE_DESC::Buffer(m_size_aligned_);

    for (auto i = 0; i < RenderEngine::kFrame_Buffer_Count; ++i)
    {
        m_buffers_[i] = DirectXResourceFactory::CreateBuffer(
            prop,
            desc,
            D3D12_RESOURCE_STATE_GENERIC_READ);

        m_current_state_[i] = D3D12_RESOURCE_STATE_GENERIC_READ;

        if (m_buffers_[i] == nullptr)
        {
            Logger::Error<ConstantBuffer>("failed to create constant buffer resource");
            return;
        }

        constexpr D3D12_RANGE unreadable_range = {0, 0};
        const auto hr = m_buffers_[i]->Map(0, &unreadable_range, &m_p_mapped_ptrs_[i]);
        if (FAILED(hr))
        {
            Logger::Error<ConstantBuffer>("failed to constant buffer mapping");
            return;
        }

        m_desc_[i] = {};
        m_desc_[i].BufferLocation = m_buffers_[i]->GetGPUVirtualAddress();
        m_desc_[i].SizeInBytes = static_cast<UINT>(m_size_aligned_);

        m_buffers_[i]->SetName(L"ConstantBuffer");
    }
}

void ConstantBuffer::UpdateBuffer(const void *data)
{
    const auto current_back_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    memcpy(m_p_mapped_ptrs_[current_back_buffer_idx], data, m_size_);
}

void ConstantBuffer::UploadBuffer(const std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav)
{
    const auto view_desc = ViewDesc();
    RenderEngine::Device()->CreateConstantBufferView(&view_desc, desc_handle->handle_cpu);
}

std::shared_ptr<DescriptorHandle> ConstantBuffer::UploadBuffer()
{
    return DescriptorHeap::Register(*this);
}

bool ConstantBuffer::IsValid()
{
    return m_buffers_[0] != nullptr;
}

bool ConstantBuffer::Transition(const D3D12_RESOURCE_STATES new_state)
{
    const auto current_back_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    if (m_current_state_[current_back_buffer_idx] == new_state)
        return false;

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_buffers_[current_back_buffer_idx].Get(), m_current_state_[current_back_buffer_idx],
        new_state);
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);

    m_current_state_[current_back_buffer_idx] = new_state;
    return true;
}

void *ConstantBuffer::GetPtr() const
{
    const auto current_back_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    return m_p_mapped_ptrs_[current_back_buffer_idx];
}
}