#include "pch.h"
#include "Texture2DArray.h"

#include "DescriptorHeap.h"
#include "RenderEngine.h"

void Texture2DArray::CreateBuffer()
{
    auto texture = m_textures_[0].CastedLock();
    D3D12_RESOURCE_DESC array_desc = {};
    array_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    array_desc.Width = texture->Width();
    array_desc.Height = texture->Height();
    array_desc.DepthOrArraySize = m_textures_.size();
    array_desc.MipLevels = texture->MipLevel();
    array_desc.Format = texture->Format();
    array_desc.SampleDesc.Count = 1;
    array_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    array_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    const auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &array_desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_p_resource_)
        );

    if (FAILED(hr))
    {
        m_is_valid_ = false;
        return;
    }

    hr = m_p_resource_->SetName(L"TextureArray");
    if (FAILED(hr))
    {
        m_is_valid_ = false;
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
            m_textures_.size()
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
void Texture2DArray::UpdateBuffer(void *data)
{}

std::shared_ptr<DescriptorHandle> Texture2DArray::UploadBuffer()
{
    return DescriptorHeap::Register(this);
}

bool Texture2DArray::CanUpdate()
{
    return false;
}

bool Texture2DArray::IsValid()
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
    view_desc.Format = m_p_resource_->GetDesc().Format;
    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    view_desc.Texture2DArray.MipLevels = m_textures_.begin()->CastedLock()->MipLevel();
    view_desc.Texture2DArray.FirstArraySlice = 0;
    view_desc.Texture2DArray.ArraySize = m_textures_.size();

    return view_desc;
}

void Texture2DArray::AddTexture(engine::AssetPtr<Texture2D> texture)
{
    m_textures_.emplace_back(texture);
    CreateBuffer();
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
    CreateBuffer();
}