#pragma once
#include "object.h"
#include "CabotEngine/Graphics/ComPtr.h"
#include "CabotEngine/Graphics/RenderEngine.h"

#include <directx/d3d12.h>

class RenderTexture : public engine::Object
{
    ComPtr<ID3D12Resource> m_RenderTargets_[RenderEngine::FRAME_BUFFER_COUNT];
    ComPtr<ID3D12DescriptorHeap> m_RTVHeap_;

public:
    void OnConstructed() override;
    void BeginRender();
    void EndRender();

    ID3D12Resource *Resouce() const;
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc();
};