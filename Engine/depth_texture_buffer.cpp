#include "pch.h"
#include "depth_texture_buffer.h"

#include "application.h"
#include "Rendering/depth_texture.h"
#include "Rendering/CabotEngine/Graphics/DirectXResourceFactory.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
DepthTextureBuffer::DepthTextureBuffer(const std::shared_ptr<DepthTexture> &depth_texture) : TextureBuffer(depth_texture)
{}

void DepthTextureBuffer::CreateBuffer()
{
    const auto device = RenderEngine::Device();

    m_width_ = Application::WindowWidth();
    m_height_ = Application::WindowHeight();
    m_format_ = DXGI_FORMAT_R32_FLOAT;
    m_mip_level_ = 1;

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    const auto hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dsv_heap_));
    if (FAILED(hr))
    {
        Logger::Error<DepthTextureBuffer>("Failed To Create DSV Heap for DepthTexture");
    }

    D3D12_CLEAR_VALUE dsv_clear_value = {};
    dsv_clear_value.Format = DXGI_FORMAT_D32_FLOAT;
    dsv_clear_value.DepthStencil.Depth = 1.0f;
    dsv_clear_value.DepthStencil.Stencil = 0;

    const auto heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    CD3DX12_RESOURCE_DESC resource_desc =
        CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, m_width_, m_height_);

    resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    resource_desc.MipLevels = 1;

    m_current_state_ = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    m_resource_ = DirectXResourceFactory::CreateBuffer(
        heap_prop,
        resource_desc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_HEAP_FLAG_NONE,
        &dsv_clear_value);

    if (m_resource_ == nullptr)
    {
        Logger::Error<DepthTexture>("Failed To Create Depth Resource");
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
    dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Flags = D3D12_DSV_FLAG_NONE;

    const D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle =
        m_dsv_heap_->GetCPUDescriptorHandleForHeapStart();

    device->CreateDepthStencilView(m_resource_.Get(), &dsv_desc, dsv_handle);
}

void DepthTextureBuffer::SetResource(const std::shared_ptr<Texture2DArray> &texture_array, int index)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    const auto device = RenderEngine::Device();
    const auto hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dsv_heap_));
    if (FAILED(hr))
    {
        Logger::Error<DepthTexture>("Failed To Create DSV Heap for DepthTexture");
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
    dsvDesc.Texture2DArray.FirstArraySlice = index;
    dsvDesc.Texture2DArray.ArraySize = 1;
    dsvDesc.Texture2DArray.MipSlice = 0;

    const D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle =
        m_dsv_heap_->GetCPUDescriptorHandleForHeapStart();
    device->CreateDepthStencilView(texture_array->Resource(), &dsvDesc, dsv_handle);

    m_resource_ = texture_array->Resource();
}

ID3D12DescriptorHeap *DepthTextureBuffer::GetHeap() const
{
    return m_dsv_heap_.Get();
}

D3D12_SHADER_RESOURCE_VIEW_DESC DepthTextureBuffer::ViewDesc()
{
    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc = {};
    view_desc.Format = DXGI_FORMAT_R32_FLOAT;
    view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    view_desc.Texture2D.MipLevels = 1;
    return view_desc;
}
}