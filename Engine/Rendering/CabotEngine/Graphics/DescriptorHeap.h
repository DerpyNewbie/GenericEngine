#pragma once

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
    UINT index;
};

class DescriptorHeap
{
    static constexpr UINT kHandleMax = 512;
    static std::shared_ptr<DescriptorHeap> m_instance_;

    bool m_IsValid_ = false;
    UINT m_IncrementSize_ = 0;
    std::vector<UINT> m_FreeIndices_;
    ComPtr<ID3D12DescriptorHeap> m_pHeap_ = nullptr;
    std::vector<std::shared_ptr<DescriptorHandle>> m_pHandles_;

    static std::shared_ptr<DescriptorHeap> Instance();

public:
    DescriptorHeap();

    static ID3D12DescriptorHeap *GetHeap();
    static std::shared_ptr<DescriptorHandle> Register(std::shared_ptr<Texture2D> texture);
    static std::shared_ptr<DescriptorHandle> Register(engine::StructuredBuffer &structured_buffer);
    static std::shared_ptr<DescriptorHandle> Register(ConstantBuffer &constant_buffer);

    static std::shared_ptr<DescriptorHandle> Allocate();
    static void Free(std::shared_ptr<DescriptorHandle> handle);

    static void Release();
};