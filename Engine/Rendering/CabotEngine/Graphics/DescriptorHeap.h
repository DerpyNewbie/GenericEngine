#pragma once
#include "ComPtr.h"
#include <directx/d3dx12.h>
#include <vector>
#include "RenderEngine.h"
#include "StructuredBuffer.h"

class ConstantBuffer;
class Texture2D;

class DescriptorHandle
{
public:
    D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;
};

class DescriptorHeap
{
public:
    DescriptorHeap(); // コンストラクタで生成する
    ID3D12DescriptorHeap *GetHeap(); // ディスクリプタヒープを返す
    std::shared_ptr<DescriptorHandle> Register(std::shared_ptr<Texture2D> texture);
    template <typename T>
    std::shared_ptr<DescriptorHandle> Register(StructuredBuffer<T> structuredBuffer);
    std::shared_ptr<DescriptorHandle> Allocate();

    void Release();

private:
    bool m_IsValid = false;
    UINT m_IncrementSize = 0;
    ComPtr<ID3D12DescriptorHeap> m_pHeap = nullptr;
    std::vector<std::shared_ptr<DescriptorHandle>> m_pHandles;
};

template <typename T>
std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(StructuredBuffer<T> structuredBuffer)
{
    auto count = m_pHandles.size();
    
    UINT HANDLE_MAX = 512;
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
    auto resource = structuredBuffer.Resource();
    auto desc = structuredBuffer.ViewDesc();
    device->CreateShaderResourceView(resource, &desc, pHandle->HandleCPU); // シェーダーリソースビュー作成

    m_pHandles.push_back(pHandle);
    return pHandle; // ハンドルを返す
}