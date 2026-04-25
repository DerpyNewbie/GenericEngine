#pragma once
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"
#include "Rendering/ibuffer.h"
#include "Rendering/shader_resource.h"

namespace engine
{
class StructuredBuffer : public IBuffer, public ShaderResource
{
public:
    explicit StructuredBuffer(const size_t stride, const size_t elem_count)
    {
        m_stride_ = stride;
        m_element_count_ = elem_count;
        m_gpu_address_ = 0;
    }

    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;

    bool CanUpdate() override
    {
        return true;
    }

    bool IsValid() override;

    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
    ID3D12Resource *Resource() override;

    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;

private:
    ComPtr<ID3D12Resource> m_default_buffer_;
    ComPtr<ID3D12Resource> m_upload_buffer_;
    D3D12_GPU_VIRTUAL_ADDRESS m_gpu_address_;
    size_t m_element_count_ = 0;
    size_t m_stride_ = 0;
    bool m_is_valid_ = false;
};
}