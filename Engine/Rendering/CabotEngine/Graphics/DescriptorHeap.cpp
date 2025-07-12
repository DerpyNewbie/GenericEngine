#include "pch.h"

#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/material_block.h"

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

    auto device = g_RenderEngine->Device();

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
    return m_pHeap_.Get();
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(std::shared_ptr<Texture2D> texture)
{
    auto pHandle = Allocate();

    auto device = g_RenderEngine->Device();
    auto resource = texture->Resource();
    auto desc = texture->ViewDesc();
    device->CreateShaderResourceView(resource, &desc, pHandle->HandleCPU); // シェーダーリソースビュー作成

    return pHandle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(engine::StructuredBuffer &structured_buffer)
{
    auto pHandle = Allocate();

    auto device = g_RenderEngine->Device();
    auto resource = structured_buffer.Resource();
    auto desc = structured_buffer.ViewDesc();
    device->CreateShaderResourceView(resource, &desc, pHandle->HandleCPU); // シェーダーリソースビュー作成

    return pHandle; // ハンドルを返す
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(ConstantBuffer &constant_buffer)
{
    auto pHandle = Allocate();

    auto view_desc = constant_buffer.ViewDesc();
    g_RenderEngine->Device()->CreateConstantBufferView(&view_desc, pHandle->HandleCPU);

    return pHandle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Allocate()
{
    UINT index;

    if (!m_FreeIndices_.empty())
    {
        // 再利用できるスロットがある
        index = m_FreeIndices_.back();
        m_FreeIndices_.pop_back();
    }
    else
    {
        if (m_pHandles_.size() >= kHandleMax)
            return nullptr;

        index = static_cast<UINT>(m_pHandles_.size());
    }

    std::shared_ptr<DescriptorHandle> pHandle = std::make_shared<DescriptorHandle>();
    pHandle->index = index;

    auto handleCPU = m_pHeap_->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += m_IncrementSize_ * index;

    auto handleGPU = m_pHeap_->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += m_IncrementSize_ * index;

    pHandle->HandleCPU = handleCPU;
    pHandle->HandleGPU = handleGPU;

    if (index < m_pHandles_.size())
    {
        m_pHandles_[index] = pHandle;
    }
    else
    {
        m_pHandles_.push_back(pHandle);
    }

    return pHandle;
}

void DescriptorHeap::Free(std::shared_ptr<DescriptorHandle> handle)
{
    if (!handle || handle->index >= m_pHandles_.size())
        return;

    m_pHandles_[handle->index] = nullptr; // スロットを無効化
    m_FreeIndices_.push_back(handle->index); // 空きとして登録
}

void DescriptorHeap::Release()
{
    m_pHandles_.clear();
    m_FreeIndices_.clear();
}