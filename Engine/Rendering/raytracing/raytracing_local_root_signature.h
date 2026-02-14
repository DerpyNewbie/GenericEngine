#pragma once

class RaytracingLocalRootSignature
{
    ComPtr<ID3D12RootSignature> m_root_signature_ = nullptr;
    ComPtr<ID3DBlob> m_signature_;

public:
    constexpr static int kPreDefinedVariableCount = 9;

    static RaytracingLocalRootSignature *Instance();
    static ID3D12RootSignature *Get();

    RaytracingLocalRootSignature();
};