#include "pch.h"

#include "IndexBuffer.h"
#include "RenderEngine.h"

engine::IndexBuffer::IndexBuffer(size_t size, const uint32_t *init_data)
{
    if (init_data == nullptr)
    {
        Logger::Error<IndexBuffer>("index buffer initialization data is null");
        return;
    }

    auto upload_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);

    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &upload_prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_upload_buffer_.GetAddressOf()));

    if (FAILED(hr))
    {
        Logger::Error<IndexBuffer>("failed to create index buffer upload resource");
        return;
    }

    void *ptr = nullptr;

    constexpr D3D12_RANGE read_range = {0, 0};
    hr = m_upload_buffer_->Map(0, &read_range, &ptr);
    if (FAILED(hr))
    {
        Logger::Error<IndexBuffer>("failed to index buffer mapping");
        m_upload_buffer_.Reset();
        return;
    }

    memcpy(ptr, init_data, size);

    m_upload_buffer_->Unmap(0, nullptr);

    const auto default_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    hr = RenderEngine::Device()->CreateCommittedResource(
        &default_prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(m_default_buffer_.GetAddressOf()));

    if (FAILED(hr))
    {
        Logger::Error<IndexBuffer>("failed to create index buffer default resource");
        m_default_buffer_.Reset();
        return;
    }

    RenderEngine::CommandList()->CopyBufferRegion(m_default_buffer_.Get(), 0, m_upload_buffer_.Get(), 0, size);

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_default_buffer_.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);

    m_view_.BufferLocation = m_default_buffer_->GetGPUVirtualAddress();
    m_view_.Format = DXGI_FORMAT_R32_UINT;
    m_view_.SizeInBytes = static_cast<UINT>(size);

    m_default_buffer_->SetName(L"IndexBuffer");
}

bool engine::IndexBuffer::IsValid() const
{
    return m_default_buffer_ != nullptr;
}

D3D12_INDEX_BUFFER_VIEW *engine::IndexBuffer::View()
{
    return &m_view_;
}