#include "pch.h"
#include "TextureCube.h"

#include "DescriptorHeap.h"
#include "RenderEngine.h"
#include "gui.h"
#include "Asset/asset_ptr.h"

namespace engine
{
void TextureCube::OnInspectorGui()
{
    for (int i = 0; i < 6; ++i)
    {
        constexpr const char *dir_labels[] = {"Right", "Left", "Top", "Bottom", "Front", "Back"};
        if (Gui::PropertyField(dir_labels[i], m_textures_[i]))
        {
            m_is_valid_ = false;
            CreateBuffer();
        }
    }
}

void TextureCube::CreateBuffer()
{
    std::array<std::shared_ptr<Texture2D>, 6> locked_textures;
    for (int i = 0; i < 6; ++i)
    {
        locked_textures[i] = m_textures_[i].CastedLock();
        if (locked_textures[i] == nullptr)
        {
            Logger::Error<TextureCube>("Texture at index %d was invalid");
            m_is_valid_ = false;
            return;
        }

        if (locked_textures[0]->Width() != locked_textures[i]->Width() ||
            locked_textures[0]->Height() != locked_textures[i]->Height())
        {
            Logger::Error<TextureCube>("Texture at index %d was not the same size as the first texture");
            m_is_valid_ = false;
            return;
        }
    }

    const D3D12_RESOURCE_DESC ref_desc = locked_textures[0]->Resource()->GetDesc();
    D3D12_RESOURCE_DESC cube_desc = ref_desc;
    cube_desc.DepthOrArraySize = 6;
    cube_desc.MipLevels = 1;
    cube_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    cube_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    const auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto hr = RenderEngine::Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &cube_desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_p_resource_)
        );

    if (FAILED(hr))
    {
        m_is_valid_ = false;
        return;
    }

    hr = m_p_resource_->SetName(L"TextureCube");
    if (FAILED(hr))
    {
        m_is_valid_ = false;
        return;
    }

    for (int i = 0; i < 6; ++i)
    {
        D3D12_TEXTURE_COPY_LOCATION src_loc = {};
        src_loc.pResource = locked_textures[i]->Resource();
        src_loc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        src_loc.SubresourceIndex = 0;

        D3D12_TEXTURE_COPY_LOCATION dst_loc = {};
        dst_loc.pResource = m_p_resource_.Get();
        dst_loc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dst_loc.SubresourceIndex = D3D12CalcSubresource(0, i, 0, 1, 6);
    }

    const auto cmd_list = RenderEngine::CommandList();
    for (int i = 0; i < 6; ++i)
    {
        D3D12_TEXTURE_COPY_LOCATION src_loc = {};
        src_loc.pResource = locked_textures[i]->Resource();
        src_loc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        src_loc.SubresourceIndex = 0;

        D3D12_TEXTURE_COPY_LOCATION dst_loc = {};
        dst_loc.pResource = m_p_resource_.Get();
        dst_loc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dst_loc.SubresourceIndex = D3D12CalcSubresource(0, i, 0, 1, 6);

        cmd_list->CopyTextureRegion(&dst_loc, 0, 0, 0, &src_loc, nullptr);
    }

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_p_resource_.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );

    cmd_list->ResourceBarrier(1, &barrier);
    m_is_valid_ = true;
}

void TextureCube::UpdateBuffer(void *data)
{
    Logger::Error<TextureCube>("UpdateBuffer is not supported");
}

std::shared_ptr<DescriptorHandle> TextureCube::UploadBuffer()
{
    return DescriptorHeap::Register(shared_from_base<TextureCube>());
}

bool TextureCube::CanUpdate()
{
    return false;
}

bool TextureCube::IsValid()
{
    return m_is_valid_;
}

bool TextureCube::SetTextures(std::array<AssetPtr<Texture2D>, 6> textures)
{
    m_textures_ = textures;
    m_is_valid_ = false;
    CreateBuffer();
    return m_is_valid_;
}

ID3D12Resource *TextureCube::Resource() const
{
    return m_p_resource_.Get();
}

D3D12_SHADER_RESOURCE_VIEW_DESC TextureCube::ViewDesc() const
{
    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc;
    view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    view_desc.Format = m_p_resource_->GetDesc().Format;
    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    view_desc.TextureCube.MipLevels = 1;
    view_desc.TextureCube.MostDetailedMip = 0;

    return view_desc;
}
}