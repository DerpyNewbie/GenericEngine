#pragma once
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"
#include "Engine/Rendering/ibuffer.h"

namespace engine
{
class StructuredBuffer : public IBuffer
{
public:
    explicit StructuredBuffer(size_t stride, size_t elem_count)
    {
        m_stride = stride;
        m_elementCount = elem_count;
    }

    void CreateBuffer() override;
    void UpdateBuffer(void *data) override;
    std::shared_ptr<DescriptorHandle> UploadBuffer() override;

    bool CanUpdate() override
    {
        return true;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc();
    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;
    ID3D12Resource *Resource();
    bool IsValid() override;

private:
    ComPtr<ID3D12Resource> m_pDefaultBuffer;
    ComPtr<ID3D12Resource> m_pUploadBuffer;
    D3D12_GPU_VIRTUAL_ADDRESS m_GpuAddress;
    size_t m_elementCount = 0;
    size_t m_stride = 0;
    bool m_IsValid = false;
};
}