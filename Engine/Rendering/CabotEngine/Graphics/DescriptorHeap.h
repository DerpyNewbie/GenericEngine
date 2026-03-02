#pragma once
#include "TextureCube.h"

class ConstantBuffer;
class Texture2D;


namespace engine
{
class ShaderResource;
class MaterialBlock;
class StructuredBuffer;
}

class DescriptorHandle
{
public:
    D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;
    uint32_t index;
};

class DescriptorHeap
{

    bool m_is_valid_ = false;
    UINT m_descriptor_size_ = 0;
    UINT m_current_offset_ = 0;
    ComPtr<ID3D12DescriptorHeap> m_heap_ = nullptr;

public:
    static constexpr UINT kHandleMax = 100000;

    static DescriptorHeap *Instance();
    
    DescriptorHeap();
    UINT DescriptorSize() const;
    ID3D12DescriptorHeap *GetHeap();

    std::shared_ptr<DescriptorHandle> Allocate();
};