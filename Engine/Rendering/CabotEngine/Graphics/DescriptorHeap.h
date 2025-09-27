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

class DescriptorHandle : enable_shared_from_base<DescriptorHandle>
{
public:
    ~DescriptorHandle();

    D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;
    D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;
    uint16_t index;
};

class DescriptorHeap
{
    friend class DescriptorHandle;
    static constexpr uint16_t kHandleMax = 512;

    bool m_is_valid_ = false;
    UINT m_increment_size_ = 0;
    ComPtr<ID3D12DescriptorHeap> m_heap_ = nullptr;
    std::vector<uint16_t> m_free_handles_;
    std::array<std::weak_ptr<DescriptorHandle>, kHandleMax> m_handles_;

    static std::shared_ptr<DescriptorHeap> Instance();

public:
    DescriptorHeap();

    static ID3D12DescriptorHeap *GetHeap();
    static std::shared_ptr<DescriptorHandle> Register(engine::ShaderResource *shader_resource);
    static std::shared_ptr<DescriptorHandle> Register(ConstantBuffer &constant_buffer);

    static std::shared_ptr<DescriptorHandle> Allocate();

    static void Release();
};