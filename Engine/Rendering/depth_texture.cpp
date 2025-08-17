#include "pch.h"
#include "depth_texture.h"

#include "application.h"
#include "CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
void DepthTexture::CreateBuffer()
{
    auto device = g_RenderEngine->Device();

    width = Application::WindowWidth();
    height = Application::WindowHeight();

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    auto hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dsv_heap_));
    if (FAILED(hr))
    {
        Logger::Error<DepthTexture>("Failed To Create RTV Heap for RenderTexture");
    }

    D3D12_CLEAR_VALUE dsvClearValue;
    dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    dsvClearValue.DepthStencil.Depth = 1.0f;
    dsvClearValue.DepthStencil.Stencil = 0;

    auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, width, height);
    hr = device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &resource_desc,
        D3D12_RESOURCE_STATE_COMMON,
        &dsvClearValue,
        IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())
        );

    if (FAILED(hr))
    {
        Logger::Error<DepthTexture>("Failed To Create Resource");
    }

    //ディスクリプタを作成
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsv_heap_->GetCPUDescriptorHandleForHeapStart();

    device->CreateDepthStencilView(m_pResource.Get(), nullptr, dsvHandle);
}
}