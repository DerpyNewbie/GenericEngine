#pragma once

class RaytracingGlobalRootSignature
{
    ComPtr<ID3D12RootSignature> m_root_signature_ = nullptr;
    ComPtr<ID3DBlob> m_signature_;

public:
    static RaytracingGlobalRootSignature *Instance();
    static ID3D12RootSignature *Get();

    RaytracingGlobalRootSignature();
};