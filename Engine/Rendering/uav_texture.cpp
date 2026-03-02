#include "pch.h"
#include "uav_texture.h"

#include "CabotEngine/Graphics/RenderEngine.h"

void UavTexture::BeginRender() const
{
    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_p_resource_.Get(),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );

    auto cmd_list = RenderEngine::CommandList();
    cmd_list->ResourceBarrier(1, &barrier);
}

void UavTexture::EndRender() const
{
    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_p_resource_.Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );

    auto cmd_list = RenderEngine::CommandList();
    cmd_list->ResourceBarrier(1, &barrier);
}

void UavTexture::CreateBuffer()
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
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(&m_p_resource_)
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
        m_p_resource_.Get(),
        nullptr,
        &uav_view_desc,
        m_uav_heap_->GetCPUDescriptorHandleForHeapStart()
    );
}

D3D12_GPU_DESCRIPTOR_HANDLE UavTexture::GetGpuHandle() const
{
    return m_uav_heap_->GetGPUDescriptorHandleForHeapStart();
}

D3D12_SHADER_RESOURCE_VIEW_DESC UavTexture::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv_desc.Texture2D.MipLevels = 1;
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    return srv_desc;
}