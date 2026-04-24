#include "pch.h"

#include "descriptor_heap.h"
#include "Texture2D.h"
#include "RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/constant_buffer.h"
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

void DescriptorHeap::Free(std::shared_ptr<DescriptorHandle> handle)
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