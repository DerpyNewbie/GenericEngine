#include "pch.h"

#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/material_block.h"

std::shared_ptr<DescriptorHeap> DescriptorHeap::m_instance_;

std::shared_ptr<DescriptorHeap> DescriptorHeap::Instance()
{
    if (!m_instance_)
    {
        m_instance_ = std::make_shared<DescriptorHeap>();
    }
    return m_instance_;
}

DescriptorHeap::DescriptorHeap()
{
    m_free_indices_.reserve(kHandleMax);
    for (UINT i = 0; i < kHandleMax; ++i)
        m_free_indices_.emplace_back(i);

    m_p_handles_.clear();
    m_p_handles_.reserve(kHandleMax);

    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.NodeMask = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = kHandleMax;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    auto device = RenderEngine::Device();

    auto hr = device->CreateDescriptorHeap(
        &desc,

        IID_PPV_ARGS(m_p_heap_.ReleaseAndGetAddressOf()));
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
    return Instance()->m_p_heap_.Get();
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(engine::ShaderResource *shader_resource)
{
    auto desc_handle = Instance()->Allocate();

    const auto device = RenderEngine::Device();
    const auto resource = shader_resource->Resource();
    const auto desc = shader_resource->ViewDesc();
    device->CreateShaderResourceView(resource, &desc, desc_handle->handle_cpu);

    return desc_handle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(ConstantBuffer &constant_buffer)
{
    auto pHandle = Instance()->Allocate();

    auto view_desc = constant_buffer.ViewDesc();
    RenderEngine::Device()->CreateConstantBufferView(&view_desc, pHandle->handle_cpu);

    return pHandle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Allocate()
{
    auto instance = Instance();
    UINT index;

    if (!m_instance_->m_free_indices_.empty())
    {
        // 再利用できるスロットがある
        index = m_instance_->m_free_indices_.front();
        instance->m_free_indices_.erase(instance->m_free_indices_.begin());
    }
    else
    {
        if (instance->m_p_handles_.size() >= kHandleMax)
            return nullptr;

        index = static_cast<UINT>(Instance()->m_p_handles_.size());
    }

    std::shared_ptr<DescriptorHandle> pHandle = std::make_shared<DescriptorHandle>();
    pHandle->index = index;

    auto handleCPU = instance->m_p_heap_->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += instance->m_increment_size_ * index;

    auto handleGPU = instance->m_p_heap_->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += instance->m_increment_size_ * index;

    pHandle->handle_cpu = handleCPU;
    pHandle->handle_gpu = handleGPU;

    if (index < instance->m_p_handles_.size())
    {
        instance->m_p_handles_[index] = pHandle;
    }
    else
    {
        m_instance_->m_p_handles_.push_back(pHandle);
    }

    return pHandle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Allocate(const uint32_t index)
{
    auto instance = Instance();
    if (index >= kHandleMax)
        return nullptr;

    auto it = std::ranges::find(instance->m_free_indices_, index);
    if (it == instance->m_free_indices_.end())
        return nullptr;

    auto p_handle = std::make_shared<DescriptorHandle>();
    p_handle->index = index;

    auto handle_cpu = instance->m_p_heap_->GetCPUDescriptorHandleForHeapStart();
    handle_cpu.ptr += instance->m_increment_size_ * index;

    auto handle_gpu = instance->m_p_heap_->GetGPUDescriptorHandleForHeapStart();
    handle_gpu.ptr += instance->m_increment_size_ * index;

    p_handle->handle_cpu = handle_cpu;
    p_handle->handle_gpu = handle_gpu;

    instance->m_free_indices_.erase(it);
    if (index < instance->m_p_handles_.size())
    {
        instance->m_p_handles_[index] = p_handle;
    }
    else
    {
        instance->m_p_handles_.push_back(p_handle);
    }
    return p_handle;
}

std::vector<std::shared_ptr<DescriptorHandle>> DescriptorHeap::AllocateLinedUp(size_t count)
{
    std::vector<std::shared_ptr<DescriptorHandle>> handles;
    handles.reserve(count);

    auto instance = Instance();
    uint32_t before_index = 0;
    uint32_t free_count = 0;
    for (auto &free_index : instance->m_free_indices_)
    {
        if (free_index != before_index + free_count)
        {
            count = 0;
            before_index = free_index;
        }

        ++free_count;

        if (free_count == count)
        {
            for (uint32_t i = before_index; i < before_index + count; ++i)
            {
                if (auto handle = Allocate(i))
                    handles.push_back(handle);
                else
                    for (const auto &h : handles)
                        Free(h);
            }
            break;
        }
    }

    return handles;
}

void DescriptorHeap::Free(const std::shared_ptr<DescriptorHandle> &handle)
{
    if (!handle || handle->index >= m_instance_->m_p_handles_.size())
        return;

    m_instance_->m_p_handles_[handle->index] = nullptr; // スロットを無効化
    m_instance_->m_free_indices_.push_back(handle->index); // 空きとして登録
}

void DescriptorHeap::Release()
{
    auto instance = Instance();
    instance->m_p_handles_.clear();
    instance->m_free_indices_.clear();
}