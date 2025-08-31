#include "pch.h"

#include "IndexBuffer.h"
#include "RenderEngine.h"

engine::IndexBuffer::IndexBuffer(size_t size, const uint32_t *pInitData)
{
    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // ヒーププロパティ
    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size); // リソースの設定

    // Create resource
    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_pBuffer.GetAddressOf()));
    if (FAILED(hr))
    {
        Logger::Error<IndexBuffer>("failed to create index buffer resource");
        return;
    }

    // Setup index buffer view
    m_View = {};
    m_View.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
    m_View.Format = DXGI_FORMAT_R32_UINT;
    m_View.SizeInBytes = static_cast<UINT>(size);

    // Create mapping
    if (pInitData != nullptr)
    {
        void *ptr = nullptr;
        hr = m_pBuffer->Map(0, nullptr, &ptr);
        if (FAILED(hr))
        {
            Logger::Error<IndexBuffer>("failed to index buffer mapping");
            return;
        }

        // Copy indices data to mapped ptr
        memcpy(ptr, pInitData, size);

        // Clean up mapping
        m_pBuffer->Unmap(0, nullptr);
    }
    m_IsValid = true;
}

bool engine::IndexBuffer::IsValid() const
{
    return m_IsValid;
}

D3D12_INDEX_BUFFER_VIEW *engine::IndexBuffer::View()
{
    return &m_View;
}