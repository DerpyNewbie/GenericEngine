#include "DescriptorHeap.h"
#include "Texture2D.h"
#include <d3dx12.h>
#include "RenderEngine.h"

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

ID3D12DescriptorHeap* DescriptorHeap::GetHeap()
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
    return pHandle; // ハンドルを返す
}
