#include <directx/d3dx12_barriers.h>
#include <directx/d3dx12_core.h>
#include "TextureCube.h"
#include "RenderEngine.h"

bool TextureCube::CreateTexCube(const std::array<std::shared_ptr<Texture2D>, 6> &textures)
{
    auto cmd_list = g_RenderEngine->CommandList();

    //check textures are the same size
    auto size = textures[0]->width * textures[0]->height;
    for (int i = 1; i < textures.size(); ++i)
        if (size != textures[i]->width * textures[i]->height)
            return false;

    D3D12_RESOURCE_DESC ref_desc = textures[0]->Resource()->GetDesc();
    D3D12_RESOURCE_DESC cube_desc = ref_desc;
    cube_desc.DepthOrArraySize = 6;
    cube_desc.MipLevels = 1;
    cube_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    cube_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto hr = g_RenderEngine->Device()->CreateCommittedResource(
        &prop,
        D3D12_HEAP_FLAG_NONE,
        &cube_desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_pResource_)
        );

    if (FAILED(hr))
    {
        return false;
    }

    for (int i = 0; i < 6; ++i)
    {
        D3D12_TEXTURE_COPY_LOCATION src_loc = {};
        src_loc.pResource = textures[i]->Resource();
        src_loc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        src_loc.SubresourceIndex = 0;

        D3D12_TEXTURE_COPY_LOCATION dst_loc = {};
        dst_loc.pResource = m_pResource_.Get();
        dst_loc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dst_loc.SubresourceIndex = D3D12CalcSubresource(0, i, 0, 1, 6);

        cmd_list->CopyTextureRegion(&dst_loc, 0, 0, 0, &src_loc, nullptr);
    }

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pResource_.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
    cmd_list->ResourceBarrier(1, &barrier);
    return true;
}

ID3D12Resource *TextureCube::Resource()
{
    return m_pResource_.Get();
}

D3D12_SHADER_RESOURCE_VIEW_DESC TextureCube::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc;
    view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    view_desc.Format = m_pResource_->GetDesc().Format;
    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    view_desc.TextureCube.MipLevels = 1;
    view_desc.TextureCube.MostDetailedMip = 0;

    return view_desc;
}