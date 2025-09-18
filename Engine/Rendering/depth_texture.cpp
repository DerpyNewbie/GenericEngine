#include "pch.h"
#include "depth_texture.h"

#include "application.h"
#include "CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
void DepthTexture::CreateBuffer()
{
    auto device = RenderEngine::Device();

    width = Application::Instance()->WindowWidth();
    height = Application::Instance()->WindowHeight();
    format = DXGI_FORMAT_R32_FLOAT;
    mip_level = 1;

    // ---- DSV Heap ----
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    auto hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dsv_heap_));
    if (FAILED(hr))
    {
        Logger::Error<DepthTexture>("Failed To Create DSV Heap for DepthTexture");
    }

    // ---- Resource ----
    D3D12_CLEAR_VALUE dsvClearValue = {};
    dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT; // 実際のDSVフォーマット
    dsvClearValue.DepthStencil.Depth = 1.0f;
    dsvClearValue.DepthStencil.Stencil = 0;

    auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC resourceDesc =
        CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, width, height);
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    resourceDesc.MipLevels = 1;

    hr = device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        // 初期状態は DSV として使うので DEPTH_WRITE
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &dsvClearValue,
        IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())
        );

    if (FAILED(hr))
    {
        Logger::Error<DepthTexture>("Failed To Create Depth Resource");
    }

    // ---- DSV ----
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // typelessに対応するフォーマットを指定
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle =
        m_dsv_heap_->GetCPUDescriptorHandleForHeapStart();

    device->CreateDepthStencilView(m_pResource.Get(), &dsvDesc, dsvHandle);
}

void DepthTexture::BeginRender()
{
    if (m_pResource == nullptr)
        CreateBuffer();

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pResource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_DEPTH_WRITE);
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);
}

void DepthTexture::EndRender()
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pResource.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);
}

ID3D12DescriptorHeap *DepthTexture::GetHeap()
{
    return m_dsv_heap_.Get();
}
D3D12_SHADER_RESOURCE_VIEW_DESC DepthTexture::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc = {};
    view_desc.Format = DXGI_FORMAT_R32_FLOAT;
    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    view_desc.Texture2D.MipLevels = 1;
    return view_desc;
}
}