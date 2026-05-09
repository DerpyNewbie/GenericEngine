#pragma once
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"
#include "Rendering/shader_resource.h"

namespace engine
{
class StructuredBuffer : public ShaderResource
{
    ComPtr<ID3D12Resource> m_default_buffer_;
    ComPtr<ID3D12Resource> m_upload_buffer_;
    D3D12_GPU_VIRTUAL_ADDRESS m_gpu_address_;
    size_t m_element_count_ = 0;
    size_t m_stride_ = 0;
    
public:
    explicit StructuredBuffer(const size_t stride, const size_t elem_count)
    {
        m_stride_ = stride;
        m_element_count_ = elem_count;
        m_gpu_address_ = 0;
    }

    bool CreateBuffer();
    void UpdateBuffer(const void *data) const;
    std::shared_ptr<DescriptorHandle> UploadBuffer();

    bool IsValid();

    [[nodiscard]] D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
    [[nodiscard]] ID3D12Resource *Resource() override;

    [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;

};
}