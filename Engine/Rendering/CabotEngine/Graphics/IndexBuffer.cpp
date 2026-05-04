#include "pch.h"

#include "IndexBuffer.h"

#include "DirectXResourceFactory.h"
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

    m_buffer_ = DirectXResourceFactory::CreateUploadedBuffer(
        init_data,
        size);

    m_view_.BufferLocation = m_buffer_->GetGPUVirtualAddress();
    m_view_.Format = DXGI_FORMAT_R32_UINT;
    m_view_.SizeInBytes = static_cast<UINT>(size);

    m_buffer_->SetName(L"IndexBuffer");
}

bool engine::IndexBuffer::IsValid() const
{
    return m_buffer_ != nullptr;
}

D3D12_INDEX_BUFFER_VIEW *engine::IndexBuffer::View()
{
    return &m_view_;
}