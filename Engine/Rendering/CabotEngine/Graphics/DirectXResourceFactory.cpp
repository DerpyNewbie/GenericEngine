#include "pch.h"
#include "DirectXResourceFactory.h"

#include "RenderEngine.h"

void DirectXResourceFactory::ClearStoredResources(const uint32_t back_buffer_index)
{
    m_stored_resources_[back_buffer_index].clear();
}

ComPtr<ID3D12Resource> DirectXResourceFactory::CreateBuffer(const D3D12_HEAP_PROPERTIES &heap_prop, const D3D12_RESOURCE_DESC &res_desc, D3D12_RESOURCE_STATES init_state, D3D12_HEAP_FLAGS heap_flags, const D3D12_CLEAR_VALUE *clear_value)
{
    ComPtr<ID3D12Resource> resource;

    const auto hr = RenderEngine::Device()->CreateCommittedResource(
        &heap_prop,
        heap_flags,
        &res_desc,
        init_state,
        clear_value,
        IID_PPV_ARGS(resource.GetAddressOf()));

    if (FAILED(hr))
    {
        engine::Logger::Error<DirectXResourceFactory>("Failed to create buffer resource");
    }

    return resource;
}

ComPtr<ID3D12Resource> DirectXResourceFactory::CreateUploadedBuffer(const void *data, const size_t size, const D3D12_HEAP_FLAGS heap_flags, const D3D12_RESOURCE_STATES initial_state, const D3D12_CLEAR_VALUE *clear_value)
{
    const auto default_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    const auto res_desc = CD3DX12_RESOURCE_DESC::Buffer(size);
    const auto default_buffer = CreateBuffer(default_heap_prop, res_desc, D3D12_RESOURCE_STATE_COMMON, heap_flags, clear_value);

    const auto upload_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto upload_buffer = CreateBuffer(upload_heap_prop, res_desc, D3D12_RESOURCE_STATE_GENERIC_READ, heap_flags, clear_value);

    if (default_buffer == nullptr || upload_buffer == nullptr)
    {
        return nullptr;
    }

    auto current_back_buffer_index = RenderEngine::CurrentBackBufferIndex();
    m_stored_resources_[current_back_buffer_index].emplace_back(upload_buffer);
    
    void *mapped = nullptr;
    D3D12_RANGE unreadable_read_range = {0, 0};
    const auto hr = upload_buffer->Map(0, &unreadable_read_range, &mapped);

    if (FAILED(hr))
    {
        engine::Logger::Error<DirectXResourceFactory>("Failed to map upload buffer");
        return nullptr;
    }

    memcpy(mapped, data, size);
    upload_buffer->Unmap(0, nullptr);

    RenderEngine::CommandList()->CopyBufferRegion(
        default_buffer.Get(),
        0,
        upload_buffer.Get(),
        0,
        size
    );

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        default_buffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        initial_state
    );
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);

    return default_buffer;
}

void DirectXResourceFactory::ReleaseResource(ComPtr<ID3D12Resource> resource)
{
    auto current_back_buffer_index = RenderEngine::CurrentBackBufferIndex();
    m_stored_resources_[current_back_buffer_index].emplace_back(resource);
}