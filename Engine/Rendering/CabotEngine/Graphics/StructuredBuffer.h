#pragma once
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"
#include "Rendering/shader_resource.h"

namespace engine
{
class StructuredBuffer final : public BufferBase, public ShaderResource
{
    ComPtr<ID3D12Resource> m_default_resource_;
    ComPtr<ID3D12Resource> m_upload_resource_;
    ComPtr<ID3D12Resource> m_readback_resource_;
    D3D12_GPU_VIRTUAL_ADDRESS m_gpu_address_;
    size_t m_element_count_ = 0;
    size_t m_stride_ = 0;
    D3D12_RESOURCE_STATES m_current_state_;
    uint64_t m_read_back_fence_value_;

    void CreateReadBackResource();
    
public:
    ~StructuredBuffer() override;
    explicit StructuredBuffer(const size_t stride, const size_t elem_count)
    {
        m_stride_ = stride;
        m_element_count_ = elem_count;
        m_gpu_address_ = 0;
    }

    void CreateBuffer() override;
    void UpdateBuffer(const void *data) override;
    void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav = false) override;
    kGpuUploadType BufferType() const override
    {
        return kGpuBufferType_SRV;
    }
    bool IsValid() override;
    bool Transition(D3D12_RESOURCE_STATES new_state) override;

    void RequestReadBack() override;
    bool FetchBufferData(void *data) override;

    [[nodiscard]] D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc() const;
    [[nodiscard]] D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
    [[nodiscard]] ID3D12Resource *Resource() override;

    [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;

};
}