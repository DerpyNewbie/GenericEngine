#pragma once
#include "render_texture.h"
#include "texture_buffer.h"

namespace engine
{
class RenderTextureBuffer : public TextureBuffer
{
    ComPtr<ID3D12DescriptorHeap> m_rtv_heap_;

public:
    RenderTextureBuffer(const std::shared_ptr<RenderTexture> &render_texture);
    
    void CreateBuffer() override;
    void BeginRender(Color background_color);
    void EndRender() const;
    ID3D12DescriptorHeap *GetHeap() const;

    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
};
}