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
    m_FreeIndices_.reserve(kHandleMax);
    for (UINT i = 0; i < kHandleMax; ++i)
        m_FreeIndices_.emplace_back(i);

    m_pHandles_.clear();
    m_pHandles_.reserve(kHandleMax);

    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.NodeMask = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = kHandleMax;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    auto device = RenderEngine::Device();

    // ディスクリプタヒープを生成
    auto hr = device->CreateDescriptorHeap(
        &desc,

        IID_PPV_ARGS(m_pHeap_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        m_IsValid_ = false;
        return;
    }

    m_IncrementSize_ = device->GetDescriptorHandleIncrementSize(desc.Type); // ディスクリプタヒープ1個のメモリサイズを返す
    m_IsValid_ = true;
}

ID3D12DescriptorHeap *DescriptorHeap::GetHeap()
{
    return Instance()->m_pHeap_.Get();
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(std::shared_ptr<Texture2D> texture)
{
    auto pHandle = Instance()->Allocate();

    auto device = RenderEngine::Device();
    auto resource = texture->Resource();
    auto desc = texture->ViewDesc();
    device->CreateShaderResourceView(resource, &desc, pHandle->HandleCPU); // シェーダーリソースビュー作成

    return pHandle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(engine::StructuredBuffer &structured_buffer)
{
    auto pHandle = Instance()->Allocate();

    auto device = RenderEngine::Device();
    auto resource = structured_buffer.Resource();
    auto desc = structured_buffer.ViewDesc();
    device->CreateShaderResourceView(resource, &desc, pHandle->HandleCPU); // シェーダーリソースビュー作成

    return pHandle; // ハンドルを返す
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(ConstantBuffer &constant_buffer)
{
    auto pHandle = Instance()->Allocate();

    auto view_desc = constant_buffer.ViewDesc();
    RenderEngine::Device()->CreateConstantBufferView(&view_desc, pHandle->HandleCPU);

    return pHandle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(const std::shared_ptr<engine::TextureCube> &texture_cube)
{
    // FIXME: completely exact piece of code from Texture2D and StructuredBuffer. please fix.
    auto p_handle = Allocate();

    const auto resource = texture_cube->Resource();
    const auto view_desc = texture_cube->ViewDesc();
    RenderEngine::Device()->CreateShaderResourceView(resource, &view_desc, p_handle->HandleCPU);
    return p_handle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Allocate()
{
    auto instance = Instance();
    UINT index;

    if (!m_instance_->m_FreeIndices_.empty())
    {
        // 再利用できるスロットがある
        index = m_instance_->m_FreeIndices_.front();
        instance->m_FreeIndices_.erase(instance->m_FreeIndices_.begin());
    }
    else
    {
        if (instance->m_pHandles_.size() >= kHandleMax)
            return nullptr;

        index = static_cast<UINT>(Instance()->m_pHandles_.size());
    }

    std::shared_ptr<DescriptorHandle> pHandle = std::make_shared<DescriptorHandle>();
    pHandle->index = index;

    auto handleCPU = instance->m_pHeap_->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += instance->m_IncrementSize_ * index;

    auto handleGPU = instance->m_pHeap_->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += instance->m_IncrementSize_ * index;

    pHandle->HandleCPU = handleCPU;
    pHandle->HandleGPU = handleGPU;

    if (index < instance->m_pHandles_.size())
    {
        instance->m_pHandles_[index] = pHandle;
    }
    else
    {
        m_instance_->m_pHandles_.push_back(pHandle);
    }

    return pHandle;
}

void DescriptorHeap::Free(std::shared_ptr<DescriptorHandle> handle)
{
    if (!handle || handle->index >= m_instance_->m_pHandles_.size())
        return;

    m_instance_->m_pHandles_[handle->index] = nullptr; // スロットを無効化
    m_instance_->m_FreeIndices_.push_back(handle->index); // 空きとして登録
}

void DescriptorHeap::Release()
{
    auto instance = Instance();
    instance->m_pHandles_.clear();
    instance->m_FreeIndices_.clear();
}