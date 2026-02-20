#include "pch.h"
#include "uav_buffer.h"

#include "CabotEngine/Graphics/RenderEngine.h"

engine::UavBuffer::UavBuffer(const size_t size, const D3D12_RESOURCE_STATES initial_state)
{
    m_size_ = size;
    m_initial_state_ = initial_state;
}

D3D12_GPU_VIRTUAL_ADDRESS engine::UavBuffer::GetAddress() const
{
    return m_buffer_->GetGPUVirtualAddress();
}

ID3D12Resource *engine::UavBuffer::GetResource() const
{
    return m_buffer_.Get();
}

void engine::UavBuffer::CreateBuffer()
{
    auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(m_size_, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    RenderEngine::Device()->CreateCommittedResource(
        &heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &buffer_desc,
        m_initial_state_,
        nullptr,
        IID_PPV_ARGS(&m_buffer_)
    );

    m_is_valid_ = true;
}

bool engine::UavBuffer::IsValid()
{
    return m_is_valid_;
}

bool engine::UavBuffer::CanUpdate()
{
    return false;
}