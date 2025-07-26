#include "render_texture.h"

#include "logger.h"
#include "CabotEngine/Graphics/RenderEngine.h"

#include <directx/d3dx12_barriers.h>

void RenderTexture::OnConstructed()
{
    auto device = g_RenderEngine->Device();

    // RTV用のディスクリプタヒープを作成する
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = 2;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    auto hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_RTVHeap_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        engine::Logger::Error<RenderTexture>("Failed To Create RTV Heap for RenderTexture");
    }

    // ディスクリプタのサイズを取得。
    auto desc_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap_->GetCPUDescriptorHandleForHeapStart();

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    for (const auto &m_RenderTarget : m_RenderTargets_)
    {
        device->CreateRenderTargetView(m_RenderTarget.Get(), &rtvDesc, rtvHandle);
        rtvHandle.ptr += desc_size;
    }
}

void RenderTexture::BeginRender()
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_RenderTargets_[g_RenderEngine->CurrentBackBufferIndex()].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    g_RenderEngine->CommandList()->ResourceBarrier(1, &barrier);
    g_RenderEngine->SetRenderTarget(m_RenderTargets_[g_RenderEngine->CurrentBackBufferIndex()].Get(), m_RTVHeap_.Get());
}

void RenderTexture::EndRender()
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_RenderTargets_[g_RenderEngine->CurrentBackBufferIndex()].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    g_RenderEngine->CommandList()->ResourceBarrier(1, &barrier);
}

ID3D12Resource *RenderTexture::Resouce() const
{
    return m_RenderTargets_[g_RenderEngine->CurrentBackBufferIndex()].Get();
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