#pragma once
#include "RenderEngine.h"
#include "Rendering/buffer_base.h"

namespace engine
{
class ConstantBuffer : public BufferBase
{
    uint64_t m_size_aligned_;
    uint64_t m_size_;

    std::array<ComPtr<ID3D12Resource>, RenderEngine::kFrame_Buffer_Count> m_resources_;
    std::array<D3D12_CONSTANT_BUFFER_VIEW_DESC, RenderEngine::kFrame_Buffer_Count> m_desc_;
    std::array<D3D12_RESOURCE_STATES, RenderEngine::kFrame_Buffer_Count> m_current_state_;

    std::array<void *, RenderEngine::kFrame_Buffer_Count> m_p_mapped_ptrs_ = {nullptr};

public:
    ~ConstantBuffer() override;
    explicit ConstantBuffer(size_t size);

    void SetBufferSize(const size_t size)
    {
        m_size_ = size;
        constexpr size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
        m_size_aligned_ = ((size + (align - 1)) & ~(align - 1));
    }
    
    void CreateBuffer() override;
    void UpdateBuffer(const void *data) override;
    void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav = false) override;
    kGpuUploadType BufferType() const override
    {
        return kGpuBufferType_CBV;
    }
    bool IsValid() override;
    bool Transition(D3D12_RESOURCE_STATES new_state) override;

    void RequestReadBack() override;
    bool FetchBufferData(void *data) override;

    void *GetPtr() const;

    D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;
    D3D12_CONSTANT_BUFFER_VIEW_DESC ViewDesc() const;
    ID3D12Resource* Resource() override;

    template <typename T>
    T *GetPtr()
    {
        return static_cast<T *>(GetPtr());
    }
};
}