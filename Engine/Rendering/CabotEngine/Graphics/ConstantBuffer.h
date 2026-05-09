#pragma once
#include "DescriptorHeap.h"

class ConstantBuffer
{
    uint64_t m_size_aligned_;
    uint64_t m_size_;

    ComPtr<ID3D12Resource> m_buffer_;
    D3D12_CONSTANT_BUFFER_VIEW_DESC m_desc_;

    void *m_p_mapped_ptr_ = nullptr;

public:
    explicit ConstantBuffer(size_t size);

    bool CreateBuffer();
    void UpdateBuffer(const void *data) const;
    std::shared_ptr<DescriptorHandle> UploadBuffer();
    bool IsValid();

    bool CanUpdate();

    void *GetPtr() const;

    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;
    D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc() const;

    template <typename T>
    T *GetPtr()
    {
        return static_cast<T *>(GetPtr());
    }
};