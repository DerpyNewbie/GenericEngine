#include "pch.h"
#include "Texture2DArray.h"

#include "DescriptorHeap.h"
#include "RenderEngine.h"

void Texture2DArray::CopyResource()
{
    const auto texture = m_textures_[0].CastedLock();
    const auto texture_size = Vector2(static_cast<float>(texture->Width()), static_cast<float>(texture->Height()));

    if (!CreateResource(texture_size, static_cast<UINT16>(m_textures_.size()), texture->MipLevel(), texture->Format()))
    {
        return;
    }

    const auto cmd_list = RenderEngine::CommandList();

    for (UINT i = 0; i < m_textures_.size(); ++i)
    {
        D3D12_TEXTURE_COPY_LOCATION dst = {};
        dst.pResource = m_p_resource_.Get();
        dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dst.SubresourceIndex = D3D12CalcSubresource(
            0, // mip level
            i, // array slice
            0, // plane slice
            texture->MipLevel(),
            static_cast<UINT>(m_textures_.size())
            );

        // コピー元 (既存の Texture2D の mip=0)
        D3D12_TEXTURE_COPY_LOCATION src = {};
        src.pResource = m_textures_[i].CastedLock()->Resource();
        src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        src.SubresourceIndex = 0;

        auto barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
            src.pResource,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_COPY_SOURCE
            );
        cmd_list->ResourceBarrier(1, &barrier1);

        cmd_list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
        barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
            src.pResource,
            D3D12_RESOURCE_STATE_COPY_SOURCE,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
            );
        cmd_list->ResourceBarrier(1, &barrier1);
    }

    m_is_valid_ = true;

    const CD3DX12_RESOURCE_BARRIER barrier =
        CD3DX12_RESOURCE_BARRIER::Transition(m_p_resource_.Get(),
                                             D3D12_RESOURCE_STATE_COPY_DEST,
                                             D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    cmd_list->ResourceBarrier(1, &barrier);
}

bool Texture2DArray::CreateResource(const Vector2 size, const UINT16 elem_count, const UINT16 mip_level,
                                    const DXGI_FORMAT format, const D3D12_RESOURCE_FLAGS flags,
                                    D3D12_CLEAR_VALUE *clear_value)
{
    m_element_count_ = elem_count;
    m_format_ = format;
    m_mip_level_ = mip_level;

    D3D12_RESOURCE_DESC array_desc = {};
    array_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    array_desc.Width = static_cast<UINT64>(size.x);
    array_desc.Height = static_cast<UINT64>(size.y);
    array_desc.DepthOrArraySize = elem_count;
    array_desc.MipLevels = mip_level;
    array_desc.Format = format;
    array_desc.SampleDesc.Count = 1;
    array_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    array_desc.Flags = flags;

    const auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &array_desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        clear_value,
        IID_PPV_ARGS(&m_p_resource_)
        );

    if (FAILED(hr))
    {
        m_is_valid_ = false;
        return false;
    }

    hr = m_p_resource_->SetName(L"TextureArray");
    if (FAILED(hr))
    {
        m_is_valid_ = false;
    }

    m_is_valid_ = true;
    return true;
}

std::shared_ptr<DescriptorHandle> Texture2DArray::UploadBuffer()
{
    return DescriptorHeap::Register(this);
}

bool Texture2DArray::IsValid() const
{
    return m_is_valid_;
}

ID3D12Resource *Texture2DArray::Resource()
{
    return m_p_resource_.Get();
}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture2DArray::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc = {};
    view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    view_desc.Format = m_format_;
    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    view_desc.Texture2DArray.MipLevels = m_mip_level_;
    view_desc.Texture2DArray.FirstArraySlice = 0;
    view_desc.Texture2DArray.ArraySize = m_element_count_;

    return view_desc;
}

void Texture2DArray::AddTexture(engine::AssetPtr<Texture2D> texture)
{
    ++m_element_count_;
    m_textures_.emplace_back(texture);
    CopyResource();
}

void Texture2DArray::RemoveTexture(engine::AssetPtr<Texture2D> texture)
{
    std::erase_if(m_textures_, [texture](const engine::AssetPtr<Texture2D> &texture1) {
        return texture == texture1;
    });
    if (m_textures_.empty())
    {
        m_is_valid_ = false;
        return;
    }
    --m_element_count_;
    CopyResource();
}
void Texture2DArray::SetFormat(const DXGI_FORMAT format)
{
    m_format_ = format;
}