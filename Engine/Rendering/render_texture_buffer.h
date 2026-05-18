#pragma once
#include "texture_buffer.h"

namespace engine
{
class RenderTextureBuffer : public TextureBuffer
{
    ComPtr<ID3D12DescriptorHeap> m_rtv_heap_;

public:
    void CreateBuffer() override;
    void BeginRender(Color background_color);
    void EndRender() const;
    ID3D12DescriptorHeap *GetHeap() const;

    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
};
}