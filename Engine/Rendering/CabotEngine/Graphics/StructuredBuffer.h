#pragma once
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"
#include "Engine/Rendering/ibuffer.h"

namespace engine
{
class StructuredBuffer : public IBuffer
{
public:
    explicit StructuredBuffer(const size_t stride, const size_t elem_count)
    {
        m_stride = static_cast<UINT>(stride);
        m_elementCount = static_cast<UINT>(elem_count);
        m_GpuAddress = 0;
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
    UINT m_elementCount = 0;
    UINT m_stride = 0;
    bool m_IsValid = false;
};
}