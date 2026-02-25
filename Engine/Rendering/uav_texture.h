#pragma once
#include "CabotEngine/Graphics/Texture2D.h"

class UavTexture : public Texture2D
{
    ComPtr<ID3D12DescriptorHeap> m_uav_heap_;

public:
    void CreateBuffer() override;
    ID3D12DescriptorHeap *DescriptorHeap() const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const;
    D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc() override;
};