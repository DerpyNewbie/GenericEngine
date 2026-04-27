#include "pch.h"

#include "IndexBuffer.h"
#include "RenderEngine.h"

engine::IndexBuffer::IndexBuffer(size_t size, const uint32_t *p_init_data)
{
    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);

    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_p_resource_.GetAddressOf()));
    if (FAILED(hr))
    {
        Logger::Error<IndexBuffer>("failed to create index buffer resource");
        return;
    }

    m_view_.BufferLocation = m_p_resource_->GetGPUVirtualAddress();
    m_view_.Format = DXGI_FORMAT_R32_UINT;
    m_view_.SizeInBytes = static_cast<UINT>(size);

    if (p_init_data != nullptr)
    {
        void *ptr = nullptr;

        D3D12_RANGE read_range = {0, 0};
        hr = m_p_resource_->Map(0, &read_range, &ptr);
        if (FAILED(hr))
        {
            Logger::Error<IndexBuffer>("failed to index buffer mapping");
            return;
        }

        memcpy(ptr, p_init_data, size);

        m_p_resource_->Unmap(0, nullptr);
    }

    m_p_resource_->SetName(L"IndexBuffer");
}

bool engine::IndexBuffer::IsValid() const
{
    return m_p_resource_ != nullptr;
}

D3D12_INDEX_BUFFER_VIEW *engine::IndexBuffer::View()
{
    return &m_view_;
}