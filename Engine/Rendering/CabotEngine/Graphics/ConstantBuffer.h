#pragma once
#include "Rendering/buffer_base.h"

namespace engine
{
class ConstantBuffer : public BufferBase
{
    uint64_t m_size_aligned_;
    uint64_t m_size_;

    ComPtr<ID3D12Resource> m_buffer_;
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_desc_;

    void *m_p_mapped_ptr_ = nullptr;

public:
    ConstantBuffer() = default;
    explicit ConstantBuffer(size_t size);

    void SetBufferSize(const size_t size)
    {
        m_size_ = size;
        constexpr size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
        m_size_aligned_ = ((size + (align - 1)) & ~(align - 1));
    }
    
    void CreateBuffer() override;
    void UpdateBuffer(const void *data) override;
    void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    kGpuUploadType BufferType() const override
    {
        return kGpuBufferType_CBV;
    }
    bool IsValid() override;

    void *GetPtr() const;

    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;
    D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc() const;

    template <typename T>
    T *GetPtr()
    {
        return static_cast<T *>(GetPtr());
    }
};
}