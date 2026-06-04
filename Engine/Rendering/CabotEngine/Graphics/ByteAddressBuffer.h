#pragma once
#include "Rendering/buffer_base.h"
#include "Rendering/shader_resource.h"

namespace engine
{
class ByteAddressBuffer : public BufferBase, public ShaderResource
{
    ComPtr<ID3D12Resource> m_default_resource_;
    ComPtr<ID3D12Resource> m_upload_resource_;
    D3D12_GPU_VIRTUAL_ADDRESS m_gpu_address_;
    size_t m_element_count_ = 0;
    D3D12_RESOURCE_STATES m_current_state_ = D3D12_RESOURCE_STATE_COMMON;

public:
    ByteAddressBuffer(size_t elem_count);

    void CreateBuffer() override;
    void UpdateBuffer(const void *data) override;
    void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav = false) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;
    kGpuUploadType BufferType() const override
    {
        return kGpuBufferType_SRV;
    }
    bool IsValid() override;
    bool Transition(D3D12_RESOURCE_STATES new_state) override;

    [[nodiscard]] D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc() const;
    [[nodiscard]] D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
    [[nodiscard]] ID3D12Resource *Resource() override;

    [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;
};
}