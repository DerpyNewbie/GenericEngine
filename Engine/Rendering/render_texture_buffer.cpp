#include "pch.h"
#include "render_texture_buffer.h"
#include "application.h"
#include "CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
RenderTextureBuffer::RenderTextureBuffer(const std::shared_ptr<RenderTexture> &render_texture) : TextureBuffer(std::static_pointer_cast<Texture2D>(render_texture))
{}

void RenderTextureBuffer::CreateBuffer()
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

    m_width_ = Application::WindowWidth();
    m_height_ = Application::WindowHeight();

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &res_desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clearValue,
        IID_PPV_ARGS(m_buffer_.ReleaseAndGetAddressOf())
    );
    m_buffer_->SetName(L"RenderTexture");

    if (FAILED(hr))
    {
        return;
    }

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = RenderEngine::RTVHeapDesc();
    heap_desc.NumDescriptors = 1;
    hr = device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(m_rtv_heap_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        Logger::Error<RenderTextureBuffer>("Failed To Create RTV Heap for RenderTexture");
    }

    D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
    rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    device->CreateRenderTargetView(m_buffer_.Get(), &rtv_desc, m_rtv_heap_->GetCPUDescriptorHandleForHeapStart());
}

void RenderTextureBuffer::BeginRender(Color background_color)
{
    if (!m_buffer_)
    {
        CreateBuffer();
    }

    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_buffer_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);
}

void RenderTextureBuffer::EndRender() const
{
    const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_buffer_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);
}

ID3D12DescriptorHeap *RenderTextureBuffer::GetHeap() const
{
    return m_rtv_heap_.Get();
}

D3D12_SHADER_RESOURCE_VIEW_DESC RenderTextureBuffer::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srv_desc.Texture2D.MipLevels = 1;
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    return srv_desc;
}
}