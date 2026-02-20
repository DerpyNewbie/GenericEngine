#include "pch.h"
#include "uav_texture.h"

#include "CabotEngine/Graphics/RenderEngine.h"

UavTexture::UavTexture()
{
    auto device = RenderEngine::Device();

    auto default_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    UINT width = 1920;
    UINT height = 1080;

    auto uav_desc = CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_R8G8B8A8_UNORM,
        width,
        height,
        1,
        1
    );

    uav_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    device->CreateCommittedResource(
        &default_heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &uav_desc,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        nullptr,
        IID_PPV_ARGS(&m_resource_)
    );

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
    heap_desc.NumDescriptors = 1;
    heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&m_uav_heap_));

    D3D12_UNORDERED_ACCESS_VIEW_DESC uav_view_desc = {};
    uav_view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uav_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    uav_view_desc.Texture2D.MipSlice = 0;

    device->CreateUnorderedAccessView(
        m_resource_.Get(),
        nullptr,
        &uav_view_desc,
        m_uav_heap_->GetCPUDescriptorHandleForHeapStart()
    );
}