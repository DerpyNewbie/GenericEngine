#pragma once
#include "Rendering/depth_texture.h"
#include "Rendering/texture_buffer.h"
#include "Rendering/CabotEngine/Graphics/Texture2DArray.h"

namespace engine
{
class DepthTextureBuffer : public TextureBuffer
{
    ComPtr<ID3D12DescriptorHeap> m_dsv_heap_;

public:
    DepthTextureBuffer(const std::shared_ptr<DepthTexture> &depth_texture);
    
    void CreateBuffer() override;
    void BeginRender();
    void EndRender();

    void SetResource(const std::shared_ptr<Texture2DArray> &texture_array, int index);

    ID3D12DescriptorHeap *GetHeap() const;
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
};
}