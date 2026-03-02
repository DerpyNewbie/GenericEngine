#pragma once
#include "CabotEngine/Graphics/Texture2D.h"

class UavTexture : public Texture2D
{
    ComPtr<ID3D12DescriptorHeap> m_uav_heap_;

public:
    void BeginRender() const;
    void EndRender() const;
    
    void CreateBuffer() override;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const;
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
};