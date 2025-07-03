#include "pch.h"

#include "DescriptorHeap.h"
#include "Texture2D.h"
#include <directx/d3dx12.h>
#include "RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/ConstantBuffer.h"
#include "Rendering/material_block.h"

const UINT HANDLE_MAX = 512;

DescriptorHeap::DescriptorHeap()
{
    m_pHandles.clear();
    m_pHandles.reserve(HANDLE_MAX);

    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.NodeMask = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = HANDLE_MAX;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    auto device = g_RenderEngine->Device();

    // ディスクリプタヒープを生成
    auto hr = device->CreateDescriptorHeap(
        &desc,
        IID_PPV_ARGS(m_pHeap.ReleaseAndGetAddressOf()));

    if (FAILED(hr))
    {
        m_IsValid = false;
        return;
    }

    m_IncrementSize = device->GetDescriptorHandleIncrementSize(desc.Type); // ディスクリプタヒープ1個のメモリサイズを返す
    m_IsValid = true;
}

ID3D12DescriptorHeap *DescriptorHeap::GetHeap()
{
    return m_pHeap.Get();
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(std::shared_ptr<Texture2D> texture)
{
    auto count = m_pHandles.size();
    if (HANDLE_MAX <= count)
    {
        return nullptr;
    }

    std::shared_ptr<DescriptorHandle> pHandle = std::make_shared<DescriptorHandle>();

    auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart(); // ディスクリプタヒープの最初のアドレス
    handleCPU.ptr += m_IncrementSize * count; // 最初のアドレスからcount番目が今回追加されたリソースのハンドル

    auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart(); // ディスクリプタヒープの最初のアドレス
    handleGPU.ptr += m_IncrementSize * count; // 最初のアドレスからcount番目が今回追加されたリソースのハンドル

    pHandle->HandleCPU = handleCPU;
    pHandle->HandleGPU = handleGPU;

    auto device = g_RenderEngine->Device();
    auto resource = texture->Resource();
    auto desc = texture->ViewDesc();
    device->CreateShaderResourceView(resource, &desc, pHandle->HandleCPU); // シェーダーリソースビュー作成

    m_pHandles.push_back(pHandle);
    return pHandle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(std::shared_ptr<engine::MaterialBlock> material_block)
{
    auto count = m_pHandles.size();
    if (HANDLE_MAX <= count)
    {
        return nullptr;
    }

    //マテリアルの最初のハンドルさえ確保できればいいのでここで代入する
    std::shared_ptr<DescriptorHandle> pHandle = Register(material_block->params_float_buffer);
    Register(material_block->params_int_buffer);
    Register(material_block->params_vec2_buffer);
    Register(material_block->params_vec3_buffer);
    Register(material_block->params_vec4_buffer);
    Register(material_block->params_mat4_buffer);
    for (auto tex : material_block->params_tex2d | std::ranges::views::values)
    {
        Register(tex);
    }
    
    return pHandle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(ConstantBuffer& constant_buffer)
{
    auto pHandle = Allocate();

    auto view_desc = constant_buffer.ViewDesc();
    g_RenderEngine->Device()->CreateConstantBufferView(&view_desc,pHandle->HandleCPU);
    return pHandle;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Allocate()
{
    auto count = m_pHandles.size() + 1;
    
    DescriptorHandle handle = {};
    if (HANDLE_MAX <= m_pHandles.size())
    {
        return nullptr;
    }

    std::shared_ptr<DescriptorHandle> pHandle = std::make_shared<DescriptorHandle>();

    auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart(); // ディスクリプタヒープの最初のアドレス
    handleCPU.ptr += m_IncrementSize * count; // 最初のアドレスからcount番目が今回追加されたリソースのハンドル

    auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart(); // ディスクリプタヒープの最初のアドレス
    handleGPU.ptr += m_IncrementSize * count; // 最初のアドレスからcount番目が今回追加されたリソースのハンドル

    pHandle->HandleCPU = handleCPU;
    pHandle->HandleGPU = handleGPU;
    
    m_pHandles.push_back(pHandle);
    return pHandle;
}

void DescriptorHeap::Release()
{
    m_pHandles.clear();
}