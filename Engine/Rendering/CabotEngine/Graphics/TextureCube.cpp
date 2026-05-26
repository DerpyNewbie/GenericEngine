#include "pch.h"
#include "TextureCube.h"

#include "DescriptorHeap.h"
#include "DirectXResourceFactory.h"
#include "RenderEngine.h"
#include "gui.h"
#include "Asset/asset_ptr.h"
#include "Rendering/texture_collection.h"

namespace engine
{
TextureCube::~TextureCube()
{
    DirectXResourceFactory::ReleaseResource(m_resource_);
}

void TextureCube::OnInspectorGui()
{
    for (int i = 0; i < 6; ++i)
    {
        constexpr const char *dir_labels[] = { "Right", "Left", "Top", "Bottom", "Front", "Back" };
        if (Gui::PropertyField(dir_labels[i], m_textures_[i]))
        {
            m_resource_ = nullptr;
            CreateBuffer();
        }
    }
}

void TextureCube::CreateBuffer()
{
    for (int i = 0; i < 6; ++i)
    {
        if (m_textures_[i] == nullptr)
        {
            Logger::Error<TextureCube>("Texture at index %d was invalid", i);
            m_resource_ = nullptr;
            return;
        }

        if (m_textures_[0]->Width() != m_textures_[i]->Width() ||
            m_textures_[0]->Height() != m_textures_[i]->Height())
        {
            Logger::Error<TextureCube>("Texture at index %d was not the same size as the first texture", i);
            m_resource_ = nullptr;
            return;
        }
    }

    const D3D12_RESOURCE_DESC ref_desc = TextureCollection::GetTexture(m_textures_[0])->Resource()->GetDesc();
    D3D12_RESOURCE_DESC cube_desc = ref_desc;
    cube_desc.DepthOrArraySize = 6;
    cube_desc.MipLevels = 1;
    cube_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    cube_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    const auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    m_resource_ = DirectXResourceFactory::CreateBuffer(
        prop,
        cube_desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_HEAP_FLAG_NONE,
        nullptr
    );

    if (m_resource_ == nullptr)
    {
        m_resource_ = nullptr;
        return;
    }

    m_resource_->SetName(L"TextureCube");
    
    const auto cmd_list = RenderEngine::CommandList();
    for (int i = 0; i < 6; ++i)
    {
        D3D12_TEXTURE_COPY_LOCATION src_loc = {};
        const auto texture_buffer = TextureCollection::GetTexture(m_textures_[i]);
        src_loc.pResource = texture_buffer->Resource();
        src_loc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        src_loc.SubresourceIndex = 0;

        D3D12_TEXTURE_COPY_LOCATION dst_loc = {};
        dst_loc.pResource = m_resource_.Get();
        dst_loc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dst_loc.SubresourceIndex = D3D12CalcSubresource(0, i, 0, 1, 6);

        cmd_list->CopyTextureRegion(&dst_loc, 0, 0, 0, &src_loc, nullptr);
    }

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_resource_.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );

    m_current_state_ = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    cmd_list->ResourceBarrier(1, &barrier);
}

void TextureCube::UpdateBuffer(const void *data)
{
    Logger::Error<TextureCube>("UpdateBuffer is not supported");
}

void TextureCube::UploadBuffer(const std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav)
{
    const auto view_desc = ViewDesc();
    RenderEngine::Device()->CreateShaderResourceView(Resource(), &view_desc, desc_handle->handle_cpu);
}

std::shared_ptr<DescriptorHandle> TextureCube::UploadBuffer()
{
    return DescriptorHeap::Register(this);
}

bool TextureCube::IsValid()
{
    return m_resource_ != nullptr;
}

bool TextureCube::Transition(const D3D12_RESOURCE_STATES new_state)
{
    if (m_current_state_ == new_state)
        return false;

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_resource_.Get(), m_current_state_,
        new_state);
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);

    m_current_state_ = new_state;
    return true;
}

ID3D12Resource *TextureCube::Resource()
{
    if (!IsValid())
        CreateBuffer();
    return m_resource_.Get();
}

D3D12_SHADER_RESOURCE_VIEW_DESC TextureCube::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc;
    view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    view_desc.Format = m_resource_->GetDesc().Format;
    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    view_desc.TextureCube.MipLevels = 1;
    view_desc.TextureCube.MostDetailedMip = 0;

    return view_desc;
}

bool TextureCube::SetTextures(const std::array<AssetPtr<Texture2D>, 6> &textures)
{
    m_textures_ = textures;
    m_resource_ = nullptr;
    CreateBuffer();
    return IsValid();
}
}

CEREAL_REGISTER_TYPE(engine::TextureCube)