#pragma once
#include "buffer_base.h"
#include "shader_resource.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class TextureBuffer : public BufferBase, public ShaderResource
{
protected:
    ComPtr<ID3D12Resource> m_buffer_ = nullptr;

    std::vector<DirectX::PackedVector::XMCOLOR> m_tex_data_ = {};
    uint32_t m_width_ = -1;
    uint32_t m_height_ = -1;
    uint16_t m_mip_level_;
    DXGI_FORMAT m_format_;
    D3D12_RESOURCE_STATES m_current_state_;

public:
    TextureBuffer() = default;
    TextureBuffer(const std::shared_ptr<Texture2D> &texture);
    
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

    ID3D12Resource *Resource() override;
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;

};
}