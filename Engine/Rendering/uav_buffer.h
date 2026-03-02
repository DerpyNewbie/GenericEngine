#pragma once
#include "ibuffer.h"

namespace engine
{
class UavBuffer : public IBuffer
{
    UINT64 m_size_;
    D3D12_RESOURCE_STATES m_initial_state_;
    ComPtr<ID3D12Resource> m_buffer_;
    void *m_p_mapped_ptr_ = nullptr;
    bool m_is_valid_ = false;

    UavBuffer(const UavBuffer &) = delete;
    void operator =(const UavBuffer &) = delete;

public:
    UavBuffer(size_t size, D3D12_RESOURCE_STATES initial_state);
    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;
    ID3D12Resource *GetResource() const;

    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    void UploadBuffer(const DescriptorHandle &descriptor_handle) override;
    bool IsValid() override;

    bool CanUpdate() override;
};
}