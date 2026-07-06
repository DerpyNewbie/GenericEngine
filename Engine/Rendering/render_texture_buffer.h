#pragma once
#include "render_texture.h"
#include "texture_buffer.h"

namespace engine
{
class RenderTextureBuffer : public TextureBuffer
{
    bool m_is_unordered_access_;
    ComPtr<ID3D12DescriptorHeap> m_rtv_heap_;

public:
    RenderTextureBuffer(const std::shared_ptr<RenderTexture> &render_texture);
    
    void CreateBuffer() override;
    void UploadBuffer(std::shared_ptr<DescriptorHandle> desc_handle, bool is_uav = false) override;
    
    ID3D12DescriptorHeap *GetHeap() const;

    static D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc();
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
};
}