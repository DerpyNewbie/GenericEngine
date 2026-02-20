#pragma once

class UavTexture
{
    ComPtr<ID3D12Resource> m_resource_;
    ComPtr<ID3D12DescriptorHeap> m_uav_heap_;

public:
    UavTexture();
};