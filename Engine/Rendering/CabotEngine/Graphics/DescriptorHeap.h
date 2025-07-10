#pragma once
#include "RenderEngine.h"

class ConstantBuffer;
class Texture2D;


namespace engine
{
class MaterialBlock;
class StructuredBuffer;
}

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

    std::shared_ptr<DescriptorHandle> Register(engine::StructuredBuffer &structured_buffer);
    std::shared_ptr<DescriptorHandle> Register(ConstantBuffer &constant_buffer);
    std::shared_ptr<DescriptorHandle> Allocate();

    void Release();

private:
    bool m_IsValid = false;
    UINT m_IncrementSize = 0;
    ComPtr<ID3D12DescriptorHeap> m_pHeap = nullptr;
    std::vector<std::shared_ptr<DescriptorHandle>> m_pHandles;
};