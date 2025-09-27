#include "pch.h"

#include "DescriptorHeap.h"
#include "RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/material_block.h"

DescriptorHandle::~DescriptorHandle()
{}

std::shared_ptr<DescriptorHeap> DescriptorHeap::Instance()
{
    static auto instance = std::make_shared<DescriptorHeap>();
    return instance;
}

DescriptorHeap::DescriptorHeap()
{
    m_free_handles_.reserve(kHandleMax);
    for (UINT i = 0; i < kHandleMax; ++i)
        m_free_handles_.emplace_back(i);

    m_handles_.fill({});

    D3D12_DESCRIPTOR_HEAP_DESC desc;
    desc.NodeMask = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = kHandleMax;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    const auto device = RenderEngine::Device();

    // ディスクリプタヒープを生成
    auto hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_heap_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        m_is_valid_ = false;
        return;
    }

    m_increment_size_ = device->GetDescriptorHandleIncrementSize(desc.Type); // ディスクリプタヒープ1個のメモリサイズを返す
    m_is_valid_ = true;
}

ID3D12DescriptorHeap *DescriptorHeap::GetHeap()
{
    return Instance()->m_heap_.Get();
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(engine::ShaderResource *shader_resource)
{
    auto desc_handle = Instance()->Allocate();

    const auto device = RenderEngine::Device();
    const auto resource = shader_resource->Resource();
    const auto desc = shader_resource->ViewDesc();
    device->CreateShaderResourceView(resource, &desc, desc_handle->HandleCPU);

    return desc_handle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(ConstantBuffer &constant_buffer)
{
    auto pHandle = Instance()->Allocate();

    auto view_desc = constant_buffer.ViewDesc();
    RenderEngine::Device()->CreateConstantBufferView(&view_desc, pHandle->HandleCPU);

    return pHandle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Allocate()
{
    const auto instance = Instance();
    UINT index;

    const auto pos = std::ranges::find_if(instance->m_handles_, [](auto ptr) {
        return ptr == nullptr;
    });

    if (pos != instance->m_handles_.end())
    {
        index = static_cast<UINT>(std::distance(instance->m_handles_.begin(), pos));
    }
    else
    {
        if (instance->m_handles_.size() >= kHandleMax)
        {
            engine::Logger::Error<DescriptorHeap>("DescriptorHeap is full!");
            return nullptr;
        }

        index = static_cast<UINT>(instance->m_handles_.size());
    }

    auto handle = std::make_shared<DescriptorHandle>();

    auto handle_cpu = instance->m_heap_->GetCPUDescriptorHandleForHeapStart();
    handle_cpu.ptr += instance->m_increment_size_ * index;

    auto handle_gpu = instance->m_heap_->GetGPUDescriptorHandleForHeapStart();
    handle_gpu.ptr += instance->m_increment_size_ * index;

    handle->HandleCPU = handle_cpu;
    handle->HandleGPU = handle_gpu;

    if (index < instance->m_handles_.size())
    {
        instance->m_handles_[index] = handle;
    }
    else
    {
        instance->m_handles_.push_back(handle);
    }

    return handle;
}

void DescriptorHeap::Free(const std::shared_ptr<DescriptorHandle> &handle)
{
    auto instance = Instance();
    if (!handle || handle->index >= instance->m_handles_.size())
        return;

    instance->m_handles_[handle->index] = nullptr; // スロットを無効化
}

void DescriptorHeap::Release()
{
    auto instance = Instance();
    instance->m_handles_.clear();
}