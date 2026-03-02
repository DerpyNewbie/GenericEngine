#include "pch.h"

#include "DescriptorHeap.h"
#include "RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/material_block.h"

DescriptorHeap *DescriptorHeap::Instance()
{
    static auto instance = new DescriptorHeap;

    return instance;
}

DescriptorHeap::DescriptorHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.NodeMask = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = kHandleMax;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    auto device = RenderEngine::Device();

    // ディスクリプタヒープを生成
    auto hr = device->CreateDescriptorHeap(
        &desc,

        IID_PPV_ARGS(m_heap_.ReleaseAndGetAddressOf()));
    if (FAILED(hr))
    {
        m_is_valid_ = false;
        return;
    }

    m_descriptor_size_ = device->GetDescriptorHandleIncrementSize(desc.Type); // ディスクリプタヒープ1個のメモリサイズを返す
    m_is_valid_ = true;
}

UINT DescriptorHeap::DescriptorSize() const
{
    return m_descriptor_size_;
}

ID3D12DescriptorHeap *DescriptorHeap::GetHeap()
{
    return m_heap_.Get();
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Allocate()
{
    std::shared_ptr<DescriptorHandle> pHandle = std::make_shared<DescriptorHandle>();

    auto handleCPU = m_heap_->GetCPUDescriptorHandleForHeapStart();
    pHandle->HandleCPU = CD3DX12_CPU_DESCRIPTOR_HANDLE(handleCPU, m_current_offset_, m_descriptor_size_);

    auto handleGPU = m_heap_->GetGPUDescriptorHandleForHeapStart();
    pHandle->HandleGPU = CD3DX12_GPU_DESCRIPTOR_HANDLE(handleGPU, m_current_offset_, m_descriptor_size_);

    ++m_current_offset_;

    pHandle->HandleCPU = handleCPU;
    pHandle->HandleGPU = handleGPU;

    return pHandle;
}