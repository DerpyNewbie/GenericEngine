#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_render_device.h"

#include <algorithm>
#include <cstring>

#include "Rendering/CaboRenderingSystem/Core/log.h"
#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_command_list.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace CRS
{
namespace
{
constexpr uint32_t kCpuSrvCapacity = 8192;
constexpr uint32_t kCpuRtvCapacity = 256;
constexpr uint32_t kCpuDsvCapacity = 64;
constexpr uint32_t kCpuSamplerCapacity = 256;
constexpr uint32_t kGpuSrvRingCapacity = 16384;
constexpr uint32_t kPersistentSrvCapacity = 64;
constexpr uint32_t kGpuSamplerRingCapacity = 1024;

uint64_t AlignUp(const uint64_t value, const uint64_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

ResourceState InitialBufferState(const BufferUsage usage)
{
    if (HasFlag(usage, BufferUsage::Index))
        return ResourceState::IndexBuffer;
    if (HasFlag(usage, BufferUsage::Vertex) || HasFlag(usage, BufferUsage::Constant))
        return ResourceState::VertexAndConstantBuffer;
    if (HasFlag(usage, BufferUsage::Structured))
        return ResourceState::ShaderResource;
    return ResourceState::Common;
}

std::wstring ToWide(const std::string &value)
{
    return {value.begin(), value.end()};
}
}

std::unique_ptr<D3D12RenderDevice> D3D12RenderDevice::Create(const HWND hwnd, const uint32_t width,
                                                             const uint32_t height)
{
    std::unique_ptr<D3D12RenderDevice> device(new D3D12RenderDevice());
    if (!device->Init(hwnd, width, height))
        return nullptr;

    return device;
}

D3D12RenderDevice::~D3D12RenderDevice()
{
    if (m_queue_ && m_fence_)
        WaitIdle();

    for (uint32_t i = 0; i < kMaxFramesInFlight; ++i)
        RetireFrame(i);

    if (m_fence_event_)
        CloseHandle(m_fence_event_);
}

bool D3D12RenderDevice::Init(const HWND hwnd, const uint32_t width, const uint32_t height)
{
#if defined(DEBUG) || defined(_DEBUG)
    ComPtr<ID3D12Debug> debug_controller;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
        debug_controller->EnableDebugLayer();
#endif

    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory_))))
    {
        LogError("Failed to create DXGI factory");
        return false;
    }

    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device_))))
    {
        LogError("Failed to create D3D12 device");
        return false;
    }

#if defined(DEBUG) || defined(_DEBUG)
    // デバッグレイヤーのエラーと警告をログに出す
    ComPtr<ID3D12InfoQueue1> info_queue;
    if (SUCCEEDED(m_device_.As(&info_queue)))
    {
        DWORD cookie = 0;
        info_queue->RegisterMessageCallback(
            [](D3D12_MESSAGE_CATEGORY, const D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID,
               const LPCSTR description, void *) {
                if (severity <= D3D12_MESSAGE_SEVERITY_ERROR)
                    LogError("D3D12: {}", description);
                else if (severity == D3D12_MESSAGE_SEVERITY_WARNING)
                    LogWarning("D3D12: {}", description);
            },
            D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &cookie);
    }
#endif

    D3D12_COMMAND_QUEUE_DESC queue_desc = {};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (FAILED(m_device_->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_queue_))))
    {
        LogError("Failed to create command queue");
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
    swap_chain_desc.Width = width;
    swap_chain_desc.Height = height;
    swap_chain_desc.Format = ToDxgiFormat(m_back_buffer_format_);
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = kMaxFramesInFlight;
    swap_chain_desc.Scaling = DXGI_SCALING_NONE;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    ComPtr<IDXGISwapChain1> swap_chain;
    if (FAILED(m_factory_->CreateSwapChainForHwnd(m_queue_.Get(), hwnd, &swap_chain_desc, nullptr, nullptr,
        &swap_chain)) ||
        FAILED(swap_chain.As(&m_swap_chain_)))
    {
        LogError("Failed to create swap chain");
        return false;
    }
    m_frame_index_ = m_swap_chain_->GetCurrentBackBufferIndex();

    for (auto &allocator : m_allocators_)
    {
        if (FAILED(m_device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator))))
        {
            LogError("Failed to create command allocator");
            return false;
        }
    }

    ComPtr<ID3D12GraphicsCommandList> list;
    if (FAILED(m_device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_allocators_[m_frame_index_].Get(),
        nullptr, IID_PPV_ARGS(&list))))
    {
        LogError("Failed to create command list");
        return false;
    }
    list->SetName(L"CRS FrameCommandList");
    list->Close();
    m_command_list_ = std::make_unique<D3D12CommandList>(*this, list);

    if (FAILED(m_device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&m_immediate_allocator_))) ||
        FAILED(m_device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_immediate_allocator_.Get(), nullptr,
            IID_PPV_ARGS(&m_immediate_list_))))
    {
        LogError("Failed to create immediate command list");
        return false;
    }
    m_immediate_list_->SetName(L"CRS ImmediateCommandList");
    m_immediate_list_->Close();

    if (FAILED(m_device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence_))) ||
        FAILED(m_device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_immediate_fence_))))
    {
        LogError("Failed to create fence");
        return false;
    }
    m_fence_event_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    if (!m_cpu_srv_heap_.Init(m_device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kCpuSrvCapacity) ||
        !m_cpu_rtv_heap_.Init(m_device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kCpuRtvCapacity) ||
        !m_cpu_dsv_heap_.Init(m_device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kCpuDsvCapacity) ||
        !m_cpu_sampler_heap_.Init(m_device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, kCpuSamplerCapacity) ||
        !m_gpu_srv_ring_.Init(m_device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kGpuSrvRingCapacity,
                              kPersistentSrvCapacity) ||
        !m_gpu_sampler_ring_.Init(m_device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, kGpuSamplerRingCapacity))
    {
        LogError("Failed to create descriptor heaps");
        return false;
    }

    // 空きスロット用のSRVと、Samplerを指定しない場合の既定のSampler
    m_null_srv_ = m_cpu_srv_heap_.Allocate();
    D3D12_SHADER_RESOURCE_VIEW_DESC null_srv_desc = {};
    null_srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    null_srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    null_srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    null_srv_desc.Texture2D.MipLevels = 1;
    m_device_->CreateShaderResourceView(nullptr, &null_srv_desc, m_null_srv_.handle);

    m_default_sampler_ = m_cpu_sampler_heap_.Allocate();
    D3D12_SAMPLER_DESC default_sampler_desc = {};
    default_sampler_desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    default_sampler_desc.AddressU = default_sampler_desc.AddressV = default_sampler_desc.AddressW =
                                                                    D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    default_sampler_desc.MaxLOD = D3D12_FLOAT32_MAX;
    default_sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    m_device_->CreateSampler(&default_sampler_desc, m_default_sampler_.handle);

    if (!CreateRootSignature())
    {
        LogError("Failed to create root signature");
        return false;
    }

    if (!CreateBackBuffers())
    {
        LogError("Failed to create back buffers");
        return false;
    }

    LogInfo("D3D12 render device initialized");
    return true;
}

bool D3D12RenderDevice::CreateRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE resource_range;
    resource_range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, kResourceSlotCount, 0, 0);
    CD3DX12_DESCRIPTOR_RANGE frame_resource_range;
    frame_resource_range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, kFrameResourceSlotCount, 0, 1);
    CD3DX12_DESCRIPTOR_RANGE sampler_range;
    sampler_range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, kSamplerSlotCount, 0, 0);

    std::array<CD3DX12_ROOT_PARAMETER, kRootParameter_Count> parameters;
    for (uint32_t slot = 0; slot < kConstantSlotCount; ++slot)
        parameters[kRootParameter_Constants + slot].InitAsConstantBufferView(slot, 0);
    for (uint32_t slot = 0; slot < kFrameConstantSlotCount; ++slot)
        parameters[kRootParameter_FrameConstants + slot].InitAsConstantBufferView(slot, 1);
    parameters[kRootParameter_Resources].InitAsDescriptorTable(1, &resource_range);
    parameters[kRootParameter_FrameResources].InitAsDescriptorTable(1, &frame_resource_range);
    parameters[kRootParameter_Samplers].InitAsDescriptorTable(1, &sampler_range);

    // space1の固定サンプラー。s0: 影の比較用, s1: 線形Clamp
    std::array<CD3DX12_STATIC_SAMPLER_DESC, 2> static_samplers;
    static_samplers[0].Init(0, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER,
                            D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0, 1,
                            D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0,
                            D3D12_FLOAT32_MAX, D3D12_SHADER_VISIBILITY_ALL, 1);
    static_samplers[1].Init(1, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                            D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 0, 1,
                            D3D12_COMPARISON_FUNC_NEVER, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0,
                            D3D12_FLOAT32_MAX, D3D12_SHADER_VISIBILITY_ALL, 1);

    CD3DX12_ROOT_SIGNATURE_DESC desc;
    desc.Init(parameters.size(), parameters.data(), static_samplers.size(),
              static_samplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> blob;
    ComPtr<ID3DBlob> error;
    if (FAILED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &blob, &error)))
    {
        if (error)
            LogError("{}", static_cast<const char *>(error->GetBufferPointer()));
        return false;
    }

    return SUCCEEDED(m_device_->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
        IID_PPV_ARGS(&m_root_signature_)));
}

bool D3D12RenderDevice::CreateBackBuffers()
{
    for (uint32_t i = 0; i < kMaxFramesInFlight; ++i)
    {
        ComPtr<ID3D12Resource> resource;
        if (FAILED(m_swap_chain_->GetBuffer(i, IID_PPV_ARGS(&resource))))
            return false;

        resource->SetName(L"CRS BackBuffer");

        D3D12Texture texture;
        const auto resource_desc = resource->GetDesc();
        texture.desc.width = static_cast<uint32_t>(resource_desc.Width);
        texture.desc.height = resource_desc.Height;
        texture.desc.format = m_back_buffer_format_;
        texture.desc.usage = TextureUsage::RenderTarget;
        texture.desc.debug_name = "BackBuffer";
        texture.resource = resource;
        texture.state = ResourceState::Present;
        CreateTextureViews(texture);
        m_back_buffers_[i] = m_textures_.Add(std::move(texture));
    }
    return true;
}

void D3D12RenderDevice::ReleaseBackBuffers()
{
    // GPUが待機済みの前提で、遅延させずに解放します
    for (auto &handle : m_back_buffers_)
    {
        if (const auto texture = m_textures_.Get(handle))
        {
            ReleaseTextureViews(*texture);
            texture->resource.Reset();
            m_textures_.Remove(handle);
        }
        handle = {};
    }
}

void D3D12RenderDevice::WaitForFence(const uint64_t value)
{
    if (value == 0 || m_fence_->GetCompletedValue() >= value)
        return;

    m_fence_->SetEventOnCompletion(value, m_fence_event_);
    WaitForSingleObjectEx(m_fence_event_, INFINITE, FALSE);
}

void D3D12RenderDevice::RetireFrame(const uint32_t frame_index)
{
    // 解放はそのフレームのGPU処理が終わった後に行う
    auto releases = std::move(m_deferred_releases_[frame_index]);
    m_deferred_releases_[frame_index].clear();
    for (auto &release : releases)
        release();

    // アップロード用リングを巻き戻す。最初のページだけ残す
    auto &pages = m_upload_pages_[frame_index];
    if (pages.size() > 1)
        pages.resize(1);
    for (auto &page : pages)
        page.offset = 0;

    // 他のフレームで書き込まれたUploadバッファの内容を、このフレームのコピーにも反映する
    const uint32_t frame_bit = 1u << frame_index;
    std::erase_if(m_pending_buffers_, [&](const BufferHandle handle) {
        const auto buffer = m_buffers_.Get(handle);
        if (buffer == nullptr)
            return true;

        if (buffer->pending_frame_mask & frame_bit)
        {
            std::memcpy(buffer->mapped[frame_index], buffer->shadow.data(), buffer->shadow.size());
            buffer->pending_frame_mask &= ~frame_bit;
        }
        return buffer->pending_frame_mask == 0;
    });
}

void D3D12RenderDevice::DeferRelease(std::function<void()> release)
{
    m_deferred_releases_[m_frame_index_].push_back(std::move(release));
}

void D3D12RenderDevice::ExecuteImmediate(const std::function<void(ID3D12GraphicsCommandList *)> &record)
{
    m_immediate_allocator_->Reset();
    m_immediate_list_->Reset(m_immediate_allocator_.Get(), nullptr);

    record(m_immediate_list_.Get());

    m_immediate_list_->Close();
    ID3D12CommandList *lists[] = {m_immediate_list_.Get()};
    m_queue_->ExecuteCommandLists(1, lists);

    const auto fence_value = ++m_immediate_fence_value_;
    m_queue_->Signal(m_immediate_fence_.Get(), fence_value);
    if (m_immediate_fence_->GetCompletedValue() < fence_value)
    {
        m_immediate_fence_->SetEventOnCompletion(fence_value, m_fence_event_);
        WaitForSingleObjectEx(m_fence_event_, INFINITE, FALSE);
    }
}

void D3D12RenderDevice::CreateTextureViews(D3D12Texture &texture)
{
    const auto &desc = texture.desc;
    const auto format = ToDxgiFormat(desc.format);
    const bool multisampled = desc.sample_count > 1;

    if (HasFlag(desc.usage, TextureUsage::ShaderResource))
    {
        texture.srv = m_cpu_srv_heap_.Allocate();
        // 深度テクスチャはR32_FLOATとして読む
        const auto srv_format = desc.format == Format::D32_Float ? DXGI_FORMAT_R32_FLOAT : format;
        const auto srv_desc = MakeSrvDesc(srv_format, desc.dimension, desc.array_size, multisampled,
                                          texture.resource->GetDesc().MipLevels);
        m_device_->CreateShaderResourceView(texture.resource.Get(), &srv_desc, texture.srv.handle);
    }

    if (HasFlag(desc.usage, TextureUsage::RenderTarget))
    {
        texture.rtv = m_cpu_rtv_heap_.Allocate();
        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
        rtv_desc.Format = format;
        rtv_desc.ViewDimension = multisampled ? D3D12_RTV_DIMENSION_TEXTURE2DMS : D3D12_RTV_DIMENSION_TEXTURE2D;
        m_device_->CreateRenderTargetView(texture.resource.Get(), &rtv_desc, texture.rtv.handle);
    }

    if (HasFlag(desc.usage, TextureUsage::DepthStencil))
    {
        texture.dsv = m_cpu_dsv_heap_.Allocate();
        D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
        dsv_desc.Format = format;
        if (desc.dimension == TextureDimension::Texture2DArray)
        {
            dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
            dsv_desc.Texture2DArray.ArraySize = desc.array_size;
        }
        else
        {
            dsv_desc.ViewDimension = multisampled ? D3D12_DSV_DIMENSION_TEXTURE2DMS : D3D12_DSV_DIMENSION_TEXTURE2D;
        }
        m_device_->CreateDepthStencilView(texture.resource.Get(), &dsv_desc, texture.dsv.handle);

        // 配列はスライスごとにも作る（影など、1枚ずつ描くため）
        if (desc.dimension == TextureDimension::Texture2DArray)
        {
            for (uint32_t slice = 0; slice < desc.array_size; ++slice)
            {
                auto descriptor = m_cpu_dsv_heap_.Allocate();
                D3D12_DEPTH_STENCIL_VIEW_DESC slice_desc = {};
                slice_desc.Format = format;
                slice_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
                slice_desc.Texture2DArray.FirstArraySlice = slice;
                slice_desc.Texture2DArray.ArraySize = 1;
                m_device_->CreateDepthStencilView(texture.resource.Get(), &slice_desc, descriptor.handle);
                texture.dsv_slices.push_back(descriptor);
            }
        }
    }
}

D3D12_SHADER_RESOURCE_VIEW_DESC D3D12RenderDevice::MakeSrvDesc(const DXGI_FORMAT format,
                                                               const TextureDimension dimension,
                                                               const uint32_t array_size, const bool multisampled,
                                                               const uint32_t mip_levels)
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = format;
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    switch (dimension)
    {
    case TextureDimension::TextureCube:
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        srv_desc.TextureCube.MipLevels = mip_levels;
        break;
    case TextureDimension::Texture2DArray:
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        srv_desc.Texture2DArray.MipLevels = mip_levels;
        srv_desc.Texture2DArray.ArraySize = array_size;
        break;
    case TextureDimension::Texture2D:
    default:
        if (multisampled)
        {
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
        }
        else
        {
            srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srv_desc.Texture2D.MipLevels = mip_levels;
        }
        break;
    }
    return srv_desc;
}

void D3D12RenderDevice::CreateBufferViews(D3D12Buffer &buffer)
{
    if (!HasFlag(buffer.desc.usage, BufferUsage::Structured) || buffer.desc.stride == 0)
        return;

    const uint32_t version_count = buffer.IsVersioned() ? kMaxFramesInFlight : 1;
    for (uint32_t i = 0; i < version_count; ++i)
    {
        buffer.srv[i] = m_cpu_srv_heap_.Allocate();
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Format = DXGI_FORMAT_UNKNOWN;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.Buffer.NumElements = static_cast<UINT>(buffer.desc.size / buffer.desc.stride);
        srv_desc.Buffer.StructureByteStride = buffer.desc.stride;
        m_device_->CreateShaderResourceView(buffer.resources[i].Get(), &srv_desc, buffer.srv[i].handle);
    }
}

void D3D12RenderDevice::ReleaseTextureViews(D3D12Texture &texture)
{
    m_cpu_srv_heap_.Free(texture.srv);
    m_cpu_rtv_heap_.Free(texture.rtv);
    m_cpu_dsv_heap_.Free(texture.dsv);
    for (auto &descriptor : texture.dsv_slices)
        m_cpu_dsv_heap_.Free(descriptor);
    texture.dsv_slices.clear();
}

bool D3D12RenderDevice::AllocateUpload(const uint64_t size, const uint64_t alignment, uint8_t *&out_cpu,
                                       D3D12_GPU_VIRTUAL_ADDRESS &out_gpu)
{
    auto &pages = m_upload_pages_[m_frame_index_];

    UploadPage *page = pages.empty() ? nullptr : &pages.back();
    if (page == nullptr || AlignUp(page->offset, alignment) + size > page->size)
    {
        UploadPage new_page;
        new_page.size = std::max(kUploadPageSize, AlignUp(size, alignment));
        const auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(new_page.size);
        if (FAILED(m_device_->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &resource_desc,
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&new_page.resource))))
        {
            LogError("Failed to allocate upload page");
            return false;
        }
        new_page.resource->SetName(L"CRS UploadRing");
        new_page.resource->Map(0, nullptr, reinterpret_cast<void **>(&new_page.mapped));
        pages.push_back(std::move(new_page));
        page = &pages.back();
    }

    const auto offset = AlignUp(page->offset, alignment);
    out_cpu = page->mapped + offset;
    out_gpu = page->resource->GetGPUVirtualAddress() + offset;
    page->offset = offset + size;
    return true;
}

D3D12_GPU_VIRTUAL_ADDRESS D3D12RenderDevice::GetBufferAddress(const BufferHandle buffer_handle)
{
    const auto buffer = m_buffers_.Get(buffer_handle);
    if (buffer == nullptr)
        return 0;

    const auto version = buffer->IsVersioned() ? m_frame_index_ : 0;
    return buffer->resources[version]->GetGPUVirtualAddress();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderDevice::GetResourceSrv(const ResourceBinding &binding)
{
    if (const auto texture = m_textures_.Get(binding.texture);
        texture != nullptr && texture->srv.IsValid())
        return texture->srv.handle;

    if (const auto buffer = m_buffers_.Get(binding.buffer))
    {
        const auto version = buffer->IsVersioned() ? m_frame_index_ : 0;
        if (buffer->srv[version].IsValid())
            return buffer->srv[version].handle;
    }

    return m_null_srv_.handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderDevice::GetSamplerDescriptor(const SamplerHandle sampler_handle)
{
    if (const auto sampler = m_samplers_.Get(sampler_handle))
        return sampler->descriptor.handle;

    return m_default_sampler_.handle;
}

BufferHandle D3D12RenderDevice::CreateBuffer(const BufferDesc &desc)
{
    if (desc.size == 0)
    {
        LogError("CreateBuffer: size is 0 ({})", desc.debug_name);
        return {};
    }

    D3D12Buffer buffer;
    buffer.desc = desc;
    buffer.desc.initial_data = nullptr;

    // 定数バッファは256byte境界に揃える
    auto size = desc.size;
    if (HasFlag(desc.usage, BufferUsage::Constant))
        size = AlignUp(size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

    const auto name = ToWide(desc.debug_name.empty() ? "CRS Buffer" : desc.debug_name);

    if (desc.memory == MemoryType::Upload)
    {
        const auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(size);
        for (uint32_t i = 0; i < kMaxFramesInFlight; ++i)
        {
            if (FAILED(m_device_->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &resource_desc,
                D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer.resources[i]))))
            {
                LogError("Failed to create upload buffer ({})", desc.debug_name);
                return {};
            }
            buffer.resources[i]->SetName(name.c_str());
            buffer.resources[i]->Map(0, nullptr, reinterpret_cast<void **>(&buffer.mapped[i]));
        }
        buffer.state = ResourceState::GenericRead;
        buffer.shadow.resize(desc.size);

        if (desc.initial_data != nullptr)
        {
            std::memcpy(buffer.shadow.data(), desc.initial_data, desc.size);
            for (uint32_t i = 0; i < kMaxFramesInFlight; ++i)
                std::memcpy(buffer.mapped[i], desc.initial_data, desc.size);
        }
    }
    else if (desc.memory == MemoryType::Readback)
    {
        const auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
        const auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(size);
        if (FAILED(m_device_->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &resource_desc,
            D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&buffer.resources[0]))))
        {
            LogError("Failed to create readback buffer ({})", desc.debug_name);
            return {};
        }
        buffer.resources[0]->SetName(name.c_str());
        buffer.state = ResourceState::CopyDest;
    }
    else
    {
        const auto flags = HasFlag(desc.usage, BufferUsage::ReadWrite)
                               ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
                               : D3D12_RESOURCE_FLAG_NONE;
        const auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        const auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(size, flags);
        if (FAILED(m_device_->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &resource_desc,
            D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&buffer.resources[0]))))
        {
            LogError("Failed to create buffer ({})", desc.debug_name);
            return {};
        }
        buffer.resources[0]->SetName(name.c_str());

        const auto target_state = InitialBufferState(desc.usage);
        if (desc.initial_data != nullptr)
        {
            // Uploadヒープ経由でコピーしてから、用途に合った状態にする
            ComPtr<ID3D12Resource> staging;
            const auto upload_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            const auto staging_desc = CD3DX12_RESOURCE_DESC::Buffer(desc.size);
            if (FAILED(m_device_->CreateCommittedResource(&upload_props, D3D12_HEAP_FLAG_NONE, &staging_desc,
                D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&staging))))
            {
                LogError("Failed to create staging buffer ({})", desc.debug_name);
                return {};
            }
            void *mapped = nullptr;
            staging->Map(0, nullptr, &mapped);
            std::memcpy(mapped, desc.initial_data, desc.size);
            staging->Unmap(0, nullptr);

            const auto destination = buffer.resources[0].Get();
            ExecuteImmediate([&](ID3D12GraphicsCommandList *list) {
                const auto to_copy = CD3DX12_RESOURCE_BARRIER::Transition(destination, D3D12_RESOURCE_STATE_COMMON,
                                                                          D3D12_RESOURCE_STATE_COPY_DEST);
                list->ResourceBarrier(1, &to_copy);
                list->CopyBufferRegion(destination, 0, staging.Get(), 0, desc.size);
                const auto to_target = CD3DX12_RESOURCE_BARRIER::Transition(
                    destination, D3D12_RESOURCE_STATE_COPY_DEST, ToD3D12State(target_state));
                list->ResourceBarrier(1, &to_target);
            });
            buffer.state = target_state;
        }
        else
        {
            // 初期状態がCOMMONのバッファは、最初の使用時に暗黙的に昇格される
            buffer.state = ResourceState::Common;
        }
    }

    CreateBufferViews(buffer);
    return m_buffers_.Add(std::move(buffer));
}

TextureHandle D3D12RenderDevice::CreateTexture(const TextureDesc &desc)
{
    D3D12Texture texture;
    texture.desc = desc;
    texture.desc.initial_data = nullptr;

    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
    if (HasFlag(desc.usage, TextureUsage::RenderTarget))
        flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    if (HasFlag(desc.usage, TextureUsage::DepthStencil))
    {
        flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        if (!HasFlag(desc.usage, TextureUsage::ShaderResource))
            flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
    }

    auto resource_format = ToDxgiFormat(desc.format);
    // 深度をSRVとしても読む場合はTypelessで作る
    if (desc.format == Format::D32_Float && HasFlag(desc.usage, TextureUsage::ShaderResource))
        resource_format = DXGI_FORMAT_R32_TYPELESS;

    const auto array_size = desc.dimension == TextureDimension::TextureCube ? 6u : std::max(desc.array_size, 1u);
    texture.desc.array_size = array_size;
    const auto resource_desc = CD3DX12_RESOURCE_DESC::Tex2D(resource_format, desc.width, desc.height,
                                                            static_cast<UINT16>(array_size), 1, desc.sample_count, 0,
                                                            flags);

    D3D12_CLEAR_VALUE clear_value = {};
    const D3D12_CLEAR_VALUE *clear_value_ptr = nullptr;
    auto initial_state = ResourceState::Common;
    if (HasFlag(desc.usage, TextureUsage::DepthStencil))
    {
        clear_value.Format = ToDxgiFormat(desc.format);
        clear_value.DepthStencil.Depth = desc.clear_depth;
        clear_value_ptr = &clear_value;
        initial_state = ResourceState::DepthWrite;
    }
    else if (HasFlag(desc.usage, TextureUsage::RenderTarget))
    {
        clear_value.Format = ToDxgiFormat(desc.format);
        std::copy(desc.clear_color.begin(), desc.clear_color.end(), clear_value.Color);
        clear_value_ptr = &clear_value;
        initial_state = ResourceState::RenderTarget;
    }
    else if (desc.initial_data != nullptr)
    {
        initial_state = ResourceState::CopyDest;
    }

    const auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    if (FAILED(m_device_->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &resource_desc,
        ToD3D12State(initial_state), clear_value_ptr, IID_PPV_ARGS(&texture.resource))))
    {
        LogError("Failed to create texture ({})", desc.debug_name);
        return {};
    }
    const auto name = ToWide(desc.debug_name.empty() ? "CRS Texture" : desc.debug_name);
    texture.resource->SetName(name.c_str());
    texture.state = initial_state;

    if (desc.initial_data != nullptr)
    {
        // initial_dataには全スライス（TextureCubeは6面）が、スライスの大きさごとに続けて並んでいる
        const auto subresource_count = array_size;
        const auto upload_size = GetRequiredIntermediateSize(texture.resource.Get(), 0, subresource_count);
        ComPtr<ID3D12Resource> staging;
        const auto upload_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const auto staging_desc = CD3DX12_RESOURCE_DESC::Buffer(upload_size);
        if (FAILED(m_device_->CreateCommittedResource(&upload_props, D3D12_HEAP_FLAG_NONE, &staging_desc,
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&staging))))
        {
            LogError("Failed to create texture staging buffer ({})", desc.debug_name);
            return {};
        }

        // ブロック圧縮は4ピクセル分で1行
        const auto row_count = IsBlockCompressed(desc.format) ? (desc.height + 3) / 4 : desc.height;
        const auto slice_pitch = static_cast<LONG_PTR>(desc.initial_row_pitch) * row_count;
        std::vector<D3D12_SUBRESOURCE_DATA> subresources(subresource_count);
        for (uint32_t i = 0; i < subresource_count; ++i)
        {
            subresources[i].pData = static_cast<const uint8_t *>(desc.initial_data) + slice_pitch * i;
            subresources[i].RowPitch = desc.initial_row_pitch;
            subresources[i].SlicePitch = slice_pitch;
        }

        const auto destination = texture.resource.Get();
        ExecuteImmediate([&](ID3D12GraphicsCommandList *list) {
            UpdateSubresources(list, destination, staging.Get(), 0, 0, subresource_count, subresources.data());
            const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                destination, D3D12_RESOURCE_STATE_COPY_DEST, ToD3D12State(ResourceState::ShaderResource));
            list->ResourceBarrier(1, &barrier);
        });
        texture.state = ResourceState::ShaderResource;
    }

    CreateTextureViews(texture);
    return m_textures_.Add(std::move(texture));
}

ShaderHandle D3D12RenderDevice::CreateShader(const ShaderSource &source)
{
    D3D12Shader shader;
    std::string error_message;
    if (!m_shader_compiler_.Compile(source, shader.bytecode, shader.reflection, error_message))
    {
        LogError("Failed to compile shader '{}' ({}):\n{}", std::string(source.file_path.begin(), source.file_path.end()),
                 std::string(source.entry_point.begin(), source.entry_point.end()), error_message);
        return {};
    }

    return m_shaders_.Add(std::move(shader));
}

PipelineStateHandle D3D12RenderDevice::CreatePipelineState(const PipelineStateDesc &desc)
{
    const auto hash = desc.Hash();
    const auto [begin, end] = m_pipeline_state_cache_.equal_range(hash);
    for (auto itr = begin; itr != end; ++itr)
    {
        if (const auto cached = m_pipeline_states_.Get(itr->second);
            cached != nullptr && cached->desc == desc)
            return itr->second;
    }

    const auto vertex_shader = m_shaders_.Get(desc.vertex_shader);
    if (vertex_shader == nullptr)
    {
        LogError("CreatePipelineState: invalid vertex shader");
        return {};
    }
    const auto pixel_shader = m_shaders_.Get(desc.pixel_shader);

    std::vector<D3D12_INPUT_ELEMENT_DESC> input_elements;
    input_elements.reserve(desc.vertex_layout.attributes.size());
    for (const auto &attribute : desc.vertex_layout.attributes)
    {
        input_elements.push_back({
            attribute.semantic.c_str(), attribute.semantic_index, ToDxgiFormat(attribute.format), 0,
            attribute.offset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        });
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
    pso_desc.pRootSignature = m_root_signature_.Get();
    pso_desc.VS = {vertex_shader->bytecode.data(), vertex_shader->bytecode.size()};
    if (pixel_shader != nullptr)
        pso_desc.PS = {pixel_shader->bytecode.data(), pixel_shader->bytecode.size()};
    pso_desc.InputLayout = {input_elements.data(), static_cast<UINT>(input_elements.size())};
    pso_desc.BlendState = ToD3D12Blend(desc.render_state.blend);
    pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pso_desc.RasterizerState.CullMode = ToD3D12Cull(desc.render_state.cull);
    // エンジンは右手系（行列もメッシュも）なので、反時計回りを表面とする
    pso_desc.RasterizerState.FrontCounterClockwise = TRUE;
    pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pso_desc.DepthStencilState.DepthEnable = desc.render_state.depth.test_enable && desc.depth_format != Format::Unknown;
    pso_desc.DepthStencilState.DepthWriteMask = desc.render_state.depth.write_enable
                                                    ? D3D12_DEPTH_WRITE_MASK_ALL
                                                    : D3D12_DEPTH_WRITE_MASK_ZERO;
    pso_desc.DepthStencilState.DepthFunc = ToD3D12Compare(desc.render_state.depth.compare);
    pso_desc.SampleMask = UINT_MAX;
    pso_desc.PrimitiveTopologyType = ToD3D12TopologyType(desc.topology);
    pso_desc.NumRenderTargets = desc.render_target_count;
    for (uint32_t i = 0; i < desc.render_target_count; ++i)
        pso_desc.RTVFormats[i] = ToDxgiFormat(desc.render_target_formats[i]);
    pso_desc.DSVFormat = ToDxgiFormat(desc.depth_format);
    pso_desc.SampleDesc.Count = desc.sample_count;

    D3D12PipelineState pipeline_state;
    pipeline_state.desc = desc;
    if (FAILED(m_device_->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pipeline_state.pipeline_state))))
    {
        LogError("Failed to create pipeline state");
        return {};
    }

    const auto handle = m_pipeline_states_.Add(std::move(pipeline_state));
    m_pipeline_state_cache_.emplace(hash, handle);
    return handle;
}

SamplerHandle D3D12RenderDevice::CreateSampler(const SamplerDesc &desc)
{
    D3D12Sampler sampler;
    sampler.desc = desc;
    sampler.descriptor = m_cpu_sampler_heap_.Allocate();

    D3D12_SAMPLER_DESC sampler_desc = {};
    sampler_desc.Filter = ToD3D12Filter(desc.filter);
    sampler_desc.AddressU = sampler_desc.AddressV = sampler_desc.AddressW = ToD3D12Address(desc.address);
    sampler_desc.MaxAnisotropy = desc.max_anisotropy;
    sampler_desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler_desc.MaxLOD = D3D12_FLOAT32_MAX;
    m_device_->CreateSampler(&sampler_desc, sampler.descriptor.handle);

    return m_samplers_.Add(std::move(sampler));
}

const ShaderReflection &D3D12RenderDevice::GetReflection(const ShaderHandle shader) const
{
    if (const auto found = m_shaders_.Get(shader))
        return found->reflection;

    return m_empty_reflection_;
}

TextureDesc D3D12RenderDevice::GetTextureDesc(const TextureHandle texture) const
{
    if (const auto found = m_textures_.Get(texture))
        return found->desc;

    return {};
}

void D3D12RenderDevice::Destroy(const BufferHandle buffer_handle)
{
    const auto buffer = m_buffers_.Get(buffer_handle);
    if (buffer == nullptr)
        return;

    auto resources = buffer->resources;
    auto srv = buffer->srv;
    m_buffers_.Remove(buffer_handle);
    DeferRelease([this, resources, srv]() mutable {
        for (auto &descriptor : srv)
            m_cpu_srv_heap_.Free(descriptor);
        // resourcesはここで解放される
        (void)resources;
    });
}

void D3D12RenderDevice::Destroy(const TextureHandle texture_handle)
{
    const auto texture = m_textures_.Get(texture_handle);
    if (texture == nullptr)
        return;

    auto moved = std::move(*texture);
    m_textures_.Remove(texture_handle);
    DeferRelease([this, moved = std::move(moved)]() mutable {
        ReleaseTextureViews(moved);
    });
}

void D3D12RenderDevice::Destroy(const ShaderHandle shader)
{
    // バイトコードはPSO作成時にしか使わないので、すぐに破棄してよい
    m_shaders_.Remove(shader);
}

void D3D12RenderDevice::Destroy(const PipelineStateHandle pipeline_state_handle)
{
    const auto pipeline_state = m_pipeline_states_.Get(pipeline_state_handle);
    if (pipeline_state == nullptr)
        return;

    std::erase_if(m_pipeline_state_cache_, [&](const auto &pair) {
        return pair.second == pipeline_state_handle;
    });

    auto native = pipeline_state->pipeline_state;
    m_pipeline_states_.Remove(pipeline_state_handle);
    DeferRelease([native]() {});
}

void D3D12RenderDevice::Destroy(const SamplerHandle sampler_handle)
{
    const auto sampler = m_samplers_.Get(sampler_handle);
    if (sampler == nullptr)
        return;

    auto descriptor = sampler->descriptor;
    m_samplers_.Remove(sampler_handle);
    DeferRelease([this, descriptor]() mutable {
        m_cpu_sampler_heap_.Free(descriptor);
    });
}

void D3D12RenderDevice::UpdateBuffer(const BufferHandle buffer_handle, const void *data, const uint64_t size,
                                     const uint64_t offset)
{
    const auto buffer = m_buffers_.Get(buffer_handle);
    if (buffer == nullptr)
    {
        LogError("UpdateBuffer: invalid buffer handle");
        return;
    }

    if (!buffer->IsVersioned())
    {
        LogError("UpdateBuffer: only Upload memory buffers can be updated from CPU ({})", buffer->desc.debug_name);
        return;
    }

    if (offset + size > buffer->desc.size)
    {
        LogError("UpdateBuffer: out of range ({})", buffer->desc.debug_name);
        return;
    }

    std::memcpy(buffer->shadow.data() + offset, data, size);
    std::memcpy(buffer->mapped[m_frame_index_] + offset, data, size);

    // 他のフレームのコピーは、そのフレームのGPU処理が終わってから反映する
    constexpr uint32_t all_frames = (1u << kMaxFramesInFlight) - 1;
    const uint32_t mask = all_frames & ~(1u << m_frame_index_);
    if (buffer->pending_frame_mask == 0 && mask != 0)
        m_pending_buffers_.push_back(buffer_handle);
    buffer->pending_frame_mask |= mask;
}

ReadbackTicket D3D12RenderDevice::RequestReadback(const BufferHandle buffer_handle)
{
    const auto buffer = m_buffers_.Get(buffer_handle);
    if (buffer == nullptr)
    {
        LogError("RequestReadback: invalid buffer handle");
        return {};
    }

    if (!HasFlag(buffer->desc.usage, BufferUsage::ReadWrite))
        LogWarning("RequestReadback: buffer '{}' is not ReadWrite. GPU never writes to it.", buffer->desc.debug_name);

    if (!m_frame_open_)
    {
        LogError("RequestReadback must be called between BeginFrame and Submit");
        return {};
    }

    PendingReadback readback;
    readback.size = buffer->desc.size;
    const auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
    const auto resource_desc = CD3DX12_RESOURCE_DESC::Buffer(readback.size);
    if (FAILED(m_device_->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &resource_desc,
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&readback.staging))))
    {
        LogError("Failed to create readback staging buffer");
        return {};
    }

    const auto version = buffer->IsVersioned() ? m_frame_index_ : 0;
    const auto list = m_command_list_->Native();
    const auto previous_state = buffer->state;
    if (!buffer->IsVersioned())
        m_command_list_->Transition(buffer_handle, ResourceState::CopySource);
    list->CopyBufferRegion(readback.staging.Get(), 0, buffer->resources[version].Get(), 0, readback.size);
    if (!buffer->IsVersioned())
        m_command_list_->Transition(buffer_handle, previous_state);

    // このフレームの終わりにシグナルされる値
    readback.fence_value = m_next_fence_value_;

    const ReadbackTicket ticket{m_next_readback_id_++};
    m_readbacks_.emplace(ticket.id, std::move(readback));
    return ticket;
}

bool D3D12RenderDevice::TryGetReadback(const ReadbackTicket ticket, std::vector<uint8_t> &out_data)
{
    const auto itr = m_readbacks_.find(ticket.id);
    if (itr == m_readbacks_.end())
        return false;

    if (!IsFenceComplete(itr->second.fence_value))
        return false;

    void *mapped = nullptr;
    const D3D12_RANGE read_range = {0, (itr->second.size)};
    itr->second.staging->Map(0, &read_range, &mapped);
    out_data.resize(itr->second.size);
    std::memcpy(out_data.data(), mapped, itr->second.size);
    constexpr D3D12_RANGE write_range = {0, 0};
    itr->second.staging->Unmap(0, &write_range);

    m_readbacks_.erase(itr);
    return true;
}

ICommandList &D3D12RenderDevice::BeginCommandList()
{
    if (!m_frame_open_)
        LogError("BeginCommandList called outside of BeginFrame / Submit");

    return *m_command_list_;
}

void D3D12RenderDevice::Submit(ICommandList &command_list)
{
    if (&command_list != m_command_list_.get() || !m_frame_open_)
    {
        LogError("Submit: command list is not open");
        return;
    }

    m_command_list_->Native()->Close();
    ID3D12CommandList *lists[] = {m_command_list_->Native()};
    m_queue_->ExecuteCommandLists(1, lists);
    m_frame_open_ = false;
}

void D3D12RenderDevice::BeginFrame()
{
    if (m_frame_open_)
    {
        LogWarning("BeginFrame called twice");
        return;
    }

    m_allocators_[m_frame_index_]->Reset();
    m_command_list_->Native()->Reset(m_allocators_[m_frame_index_].Get(), nullptr);
    m_command_list_->InvalidateState();
    m_gpu_srv_ring_.BeginFrame(m_frame_index_);
    m_gpu_sampler_ring_.BeginFrame(m_frame_index_);
    m_frame_open_ = true;
}

void D3D12RenderDevice::EndFrame()
{
    if (m_frame_open_)
        LogError("EndFrame called before Submit");

    const auto fence_value = m_next_fence_value_++;
    m_queue_->Signal(m_fence_.Get(), fence_value);
    m_frame_fence_values_[m_frame_index_] = fence_value;
}

void D3D12RenderDevice::Present()
{
    m_swap_chain_->Present(1, 0);

    // 次のフレームの準備。そのバッファを前回使ったフレームのGPU処理が終わるまで待つ
    m_frame_index_ = m_swap_chain_->GetCurrentBackBufferIndex();
    WaitForFence(m_frame_fence_values_[m_frame_index_]);
    RetireFrame(m_frame_index_);
}

TextureHandle D3D12RenderDevice::GetBackBuffer() const
{
    return m_back_buffers_[m_frame_index_];
}

Format D3D12RenderDevice::GetBackBufferFormat() const
{
    return m_back_buffer_format_;
}

uint32_t D3D12RenderDevice::GetFrameIndex() const
{
    return m_frame_index_;
}

void D3D12RenderDevice::Resize(const uint32_t width, const uint32_t height)
{
    if (width == 0 || height == 0)
        return;

    WaitIdle();
    ReleaseBackBuffers();

    if (FAILED(m_swap_chain_->ResizeBuffers(kMaxFramesInFlight, width, height, DXGI_FORMAT_UNKNOWN,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)))
    {
        LogError("Failed to resize swap chain");
    }

    m_frame_index_ = m_swap_chain_->GetCurrentBackBufferIndex();
    CreateBackBuffers();
}

void D3D12RenderDevice::WaitIdle()
{
    const auto fence_value = m_next_fence_value_++;
    m_queue_->Signal(m_fence_.Get(), fence_value);
    WaitForFence(fence_value);
}

ID3D12Device6 *D3D12RenderDevice::NativeDevice() const
{
    return m_device_.Get();
}

ID3D12CommandQueue *D3D12RenderDevice::NativeQueue() const
{
    return m_queue_.Get();
}

ID3D12GraphicsCommandList *D3D12RenderDevice::NativeCommandList() const
{
    return m_command_list_->Native();
}

ID3D12Resource *D3D12RenderDevice::NativeResource(const TextureHandle texture)
{
    const auto found = m_textures_.Get(texture);
    return found ? found->resource.Get() : nullptr;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderDevice::GetRtv(const TextureHandle texture)
{
    const auto found = m_textures_.Get(texture);
    return found ? found->rtv.handle : D3D12_CPU_DESCRIPTOR_HANDLE{};
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12RenderDevice::GetDsv(const TextureHandle texture, const uint32_t slice)
{
    const auto found = m_textures_.Get(texture);
    if (found == nullptr)
        return {};

    if (slice != kAllSlices && slice < found->dsv_slices.size())
        return found->dsv_slices[slice].handle;

    return found->dsv.handle;
}

TextureHandle D3D12RenderDevice::ImportTexture(ID3D12Resource *resource, const ResourceState fixed_state,
                                               const TextureDimension dimension)
{
    if (resource == nullptr)
        return {};

    const auto resource_desc = resource->GetDesc();

    D3D12Texture texture;
    texture.desc.width = static_cast<uint32_t>(resource_desc.Width);
    texture.desc.height = resource_desc.Height;
    texture.desc.sample_count = resource_desc.SampleDesc.Count;
    texture.desc.format = FromDxgiFormat(resource_desc.Format);
    texture.desc.array_size = resource_desc.DepthOrArraySize;
    texture.desc.dimension = dimension;
    texture.desc.usage = TextureUsage::ShaderResource;
    texture.desc.debug_name = "Imported";
    texture.resource = resource;
    texture.state = fixed_state;
    texture.fixed_state = true;

    // フォーマットが自前の型にない場合も、元のフォーマットでSRVを作る
    texture.srv = m_cpu_srv_heap_.Allocate();
    const auto srv_desc = MakeSrvDesc(resource_desc.Format, dimension, resource_desc.DepthOrArraySize,
                                      resource_desc.SampleDesc.Count > 1, resource_desc.MipLevels);
    m_device_->CreateShaderResourceView(resource, &srv_desc, texture.srv.handle);

    return m_textures_.Add(std::move(texture));
}

GpuDescriptorRange D3D12RenderDevice::AllocatePersistentSrv()
{
    GpuDescriptorRange range;
    m_gpu_srv_ring_.AllocatePersistent(range);
    return range;
}

void D3D12RenderDevice::FreePersistentSrv(const GpuDescriptorRange &range)
{
    m_gpu_srv_ring_.FreePersistent(range);
}

D3D12_GPU_DESCRIPTOR_HANDLE D3D12RenderDevice::GetTransientSrv(const TextureHandle texture)
{
    GpuDescriptorRange range;
    if (!m_gpu_srv_ring_.Allocate(1, range))
        return {};

    m_device_->CopyDescriptorsSimple(1, range.cpu, GetResourceSrv({texture, {}}),
                                     D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    return range.gpu;
}

void D3D12RenderDevice::BindDescriptorHeaps(ID3D12GraphicsCommandList *list) const
{
    ID3D12DescriptorHeap *heaps[] = {m_gpu_srv_ring_.Heap(), m_gpu_sampler_ring_.Heap()};
    list->SetDescriptorHeaps(2, heaps);
}

uint64_t D3D12RenderDevice::NextFenceValue() const
{
    return m_next_fence_value_;
}

bool D3D12RenderDevice::IsFenceComplete(const uint64_t fence_value) const
{
    return m_fence_ != nullptr && m_fence_->GetCompletedValue() >= fence_value;
}
}