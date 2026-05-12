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
    D3D12_CPU_DESCRIPTOR_HANDLE handle_cpu;
    D3D12_GPU_DESCRIPTOR_HANDLE handle_gpu;
    UINT index;
};

class DescriptorHeap
{
    static constexpr uint32_t kHandleMax = 512;
    static std::shared_ptr<DescriptorHeap> m_instance_;

    bool m_is_valid_ = false;
    uint32_t m_increment_size_ = 0;
    std::vector<uint32_t> m_free_indices_;
    ComPtr<ID3D12DescriptorHeap> m_p_heap_ = nullptr;
    std::vector<std::shared_ptr<DescriptorHandle>> m_p_handles_;

    static std::shared_ptr<DescriptorHeap> Instance();

public:
    DescriptorHeap();

    static ID3D12DescriptorHeap *GetHeap();
    static std::shared_ptr<DescriptorHandle> Register(engine::ShaderResource *shader_resource);
    static std::shared_ptr<DescriptorHandle> Register(ConstantBuffer &constant_buffer);

    static std::shared_ptr<DescriptorHandle> Allocate();
    static std::shared_ptr<DescriptorHandle> Allocate(uint32_t index);
    static std::vector<std::shared_ptr<DescriptorHandle>> AllocateLinedUp(size_t count);
    static void Free(const std::shared_ptr<DescriptorHandle> &handle);

    static void Release();
};