#pragma once
#include "CabotEngine/Graphics/Texture2D.h"
#include "CabotEngine/Graphics/Texture2DArray.h"

namespace engine
{
class DepthTexture : public Texture2D
{
    ComPtr<ID3D12DescriptorHeap> m_dsv_heap_;

public:
    void CreateBuffer() override;
    void BeginRender();
    void EndRender();

    void SetResource(const std::shared_ptr<Texture2DArray> &texture_array);

    ID3D12DescriptorHeap *GetHeap();
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
};
}