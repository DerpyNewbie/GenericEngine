#include "pch.h"

#include "render_texture.h"
#include "application.h"
#include "logger.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include <directx/d3dx12_barriers.h>

namespace engine
{
void RenderTexture::CreateBuffer()
{
    auto device = RenderEngine::Device();
    auto res_desc = RenderEngine::BBuffDesc();
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    clearValue.Color[0] = 0.5f;
    clearValue.Color[1] = 0.5f;
    clearValue.Color[2] = 0.5f;
    clearValue.Color[3] = 0.5f;

    width = Application::Instance()->WindowWidth();
    height = Application::Instance()->WindowHeight();

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &res_desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clearValue,
        IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())
        );
    m_pResource->SetName(L"RenderTexture");

    if (FAILED(hr))
    {
        return;
    }

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = RenderEngine::RTVHeapDesc();
    heap_desc.NumDescriptors = 1;
    hr = device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(m_RTVHeap_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        Logger::Error<RenderTexture>("Failed To Create RTV Heap for RenderTexture");
    }

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    device->CreateRenderTargetView(m_pResource.Get(), &rtvDesc, m_RTVHeap_->GetCPUDescriptorHandleForHeapStart());
}

void RenderTexture::BeginRender(const Color background_color)
{
    if (!m_pResource)
    {
        CreateBuffer();
    }
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pResource.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);
}

void RenderTexture::EndRender() const
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_pResource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);
}

ID3D12DescriptorHeap *RenderTexture::GetHeap()
{
    return m_RTVHeap_.Get();
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
}