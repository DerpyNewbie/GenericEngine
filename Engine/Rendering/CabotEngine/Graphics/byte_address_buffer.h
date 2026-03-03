#pragma once
#include "Rendering/ibuffer.h"
#include "Rendering/shader_resource.h"

namespace engine
{
class ByteAddressBuffer : public Object, public IBuffer, public ShaderResource
{
public:
    explicit ByteAddressBuffer(const size_t size)
    {
        m_size_ = size;
        m_gpu_address_ = 0;
    }

    ByteAddressBuffer(ID3D12Resource *resource);

    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    void UploadBuffer(const DescriptorHandle &descriptor_handle) override;

    bool CanUpdate() override
    {
        return m_can_update_;
    }

    bool IsValid() override;

    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
    ID3D12Resource *Resource() override;

    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;

private:
    ComPtr<ID3D12Resource> m_default_buffer_;
    ComPtr<ID3D12Resource> m_upload_buffer_;
    D3D12_GPU_VIRTUAL_ADDRESS m_gpu_address_;
    size_t m_size_ = 0;
    bool m_is_valid_ = false;
    bool m_can_update_ = true;
};
}