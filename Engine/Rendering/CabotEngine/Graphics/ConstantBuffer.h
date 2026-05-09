#pragma once
#include "Rendering/ibuffer.h"

class ConstantBuffer : public IBuffer
{
    uint64_t m_size_aligned_;
    uint64_t m_size_;

    ComPtr<ID3D12Resource> m_buffer_;
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_desc_;

    void *m_p_mapped_ptr_ = nullptr;

public:
    explicit ConstantBuffer(size_t size);

    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    bool IsValid() override;

    bool CanUpdate() override;

    void *GetPtr() const;

    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;
    D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc() const;

    template <typename T>
    T *GetPtr()
    {
        return static_cast<T *>(GetPtr());
    }
};