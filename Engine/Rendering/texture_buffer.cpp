#include "pch.h"
#include <stb_image.h>

#include "texture_buffer.h"
#include "CabotEngine/Graphics/DescriptorHeap.h"
#include "CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
TextureBuffer::TextureBuffer(const std::shared_ptr<Texture2D> &texture)
{
    m_tex_data_ = texture->GetPixels();

    m_format_ = texture->Format();
    m_width_ = texture->Width();
    m_height_ = texture->Height();
    m_mip_level_ = texture->MipLevel();
}

void TextureBuffer::CreateBuffer()
{
    const auto desc = CD3DX12_RESOURCE_DESC::Tex2D(
        m_format_,
        m_width_,
        m_height_,
        1,
        m_mip_level_
    );

    const auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_buffer_)
    );

    if (FAILED(hr))
    {
        Logger::Error<TextureBuffer>("failed to create texture2d resource");
        return;
    }

    m_buffer_->SetName(L"Texture");

    const D3D12_BOX dest_region = {0, 0, 0, m_width_, m_height_, 1};
    hr = m_buffer_->WriteToSubresource(
        0,
        &dest_region,
        m_tex_data_.data(),
        m_width_ * sizeof(DirectX::PackedVector::XMCOLOR),
        m_height_ * m_width_ * sizeof(DirectX::PackedVector::XMCOLOR)
    );

    if (FAILED(hr))
    {
        m_buffer_ = nullptr;
    }
}

void TextureBuffer::UpdateBuffer(const void *data)
{
    Logger::Error("Can not Update Texture2D");
}

void TextureBuffer::UploadBuffer(const std::shared_ptr<DescriptorHandle> desc_handle)
{
    const auto view_desc = ViewDesc();
    RenderEngine::Device()->CreateShaderResourceView(Resource(), &view_desc, desc_handle->handle_cpu);
}

std::shared_ptr<DescriptorHandle> TextureBuffer::UploadBuffer()
{
    return DescriptorHeap::Register(this);
}

bool TextureBuffer::IsValid()
{
    return m_buffer_ != nullptr;
}

ID3D12Resource *TextureBuffer::Resource()
{
    if (!IsValid())
    {
        CreateBuffer();
    }

    return m_buffer_ != nullptr ? m_buffer_.Get() : nullptr;
}

D3D12_SHADER_RESOURCE_VIEW_DESC TextureBuffer::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = Resource()->GetDesc().Format;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipLevels = 1;
    return desc;
}
}