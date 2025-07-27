#include "render_texture.h"
#include "application.h"
#include "logger.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include <directx/d3dx12_barriers.h>

void RenderTexture::CreateBuffer()
{
    auto device = g_RenderEngine->Device();

    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_R8G8B8A8_UNORM,
        g_app->WindowWidth(),
        g_app->WindowHeight(),
        1, // array size
        1 // mip levels
        );
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    clearValue.Color[0] = 0.5f;
    clearValue.Color[1] = 0.5f;
    clearValue.Color[2] = 0.5f;
    clearValue.Color[3] = 0.5f;

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clearValue,
        IID_PPV_ARGS(&m_pResource)
        );
    m_pResource->SetName(L"renderTexture");

    if (FAILED(hr))
    {
        return;
    }

    // RTV用のディスクリプタヒープを作成する
    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
    heap_desc.NumDescriptors = 1;
    heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    hr = device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(m_RTVHeap_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        engine::Logger::Error<RenderTexture>("Failed To Create RTV Heap for RenderTexture");
    }

    // ディスクリプタのサイズを取得。
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap_->GetCPUDescriptorHandleForHeapStart();

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    device->CreateRenderTargetView(m_pResource.Get(), &rtvDesc, rtvHandle);
}

void RenderTexture::BeginRender()
{
    if (!m_pResource)
    {
        CreateBuffer();
    }
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pResource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    g_RenderEngine->CommandList()->ResourceBarrier(1, &barrier);
    g_RenderEngine->SetRenderTarget(m_pResource.Get(), m_RTVHeap_.Get());
}

void RenderTexture::EndRender()
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pResource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    g_RenderEngine->CommandList()->ResourceBarrier(1, &barrier);
}

D3D12_SHADER_RESOURCE_VIEW_DESC RenderTexture::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv_desc.Texture2D.MipLevels = 1;
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    return srv_desc;
}