#pragma once
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class DepthTexture : public Texture2D
{
    ComPtr<ID3D12DescriptorHeap> m_dsv_heap_;

public:
    void CreateBuffer() override;
    void BeginRender();
    void EndRender();
};
}