#pragma once
#include "object.h"
#include "CabotEngine/Graphics/ComPtr.h"
#include "CabotEngine/Graphics/Texture2D.h"

#include <directx/d3d12.h>

class RenderTexture : public Texture2D
{
    ComPtr<ID3D12DescriptorHeap> m_RTVHeap_;

public:
    void CreateBuffer() override;
    void BeginRender();
    void EndRender();

    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
};