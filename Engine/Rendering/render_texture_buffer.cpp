#include "pch.h"
#include "render_texture_buffer.h"
#include "application.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/DescriptorHeap.h"

namespace engine
{
RenderTextureBuffer::RenderTextureBuffer(const std::shared_ptr<RenderTexture> &render_texture) : m_is_unordered_access_(render_texture->m_allow_uav_)
{
    m_format_ = render_texture->Format();
    m_width_ = render_texture->Width();
    m_height_ = render_texture->Height();
    m_mip_level_ = render_texture->MipLevel();
}

void RenderTextureBuffer::CreateBuffer()
{
    auto device = RenderEngine::Device();
    auto res_desc = RenderEngine::BBuffDesc();
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    m_width_ = Application::WindowWidth();
    m_height_ = Application::WindowHeight();

    res_desc.Flags = (m_is_unordered_access_ ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE) | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    m_current_state_ = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &res_desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
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

void RenderTextureBuffer::UploadBuffer(const std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav)
{
    const auto device = RenderEngine::Device();
    if (is_uav)
    {
        const auto uav_desc = UavDesc();
        device->CreateUnorderedAccessView(Resource(), nullptr, &uav_desc, desc_handle->handle_cpu);

    }
    else
    {
        const auto view_desc = ViewDesc();
        device->CreateShaderResourceView(Resource(), &view_desc, desc_handle->handle_cpu);
    }
}

ID3D12DescriptorHeap *RenderTextureBuffer::GetHeap() const
{
    return m_rtv_heap_.Get();
}

D3D12_UNORDERED_ACCESS_VIEW_DESC RenderTextureBuffer::UavDesc()
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc;
    uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uav_desc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
    uav_desc.Texture2D.MipSlice = 0;
    uav_desc.Texture2D.PlaneSlice = 0;
    return uav_desc;
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