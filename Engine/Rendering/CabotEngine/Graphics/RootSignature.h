#pragma once

namespace engine
{
enum kRootParameterIndex
{
    kWorldCBV,
    kViewProjCBV,
    kBoneSRV,
    kVertexCBV,
    kVertexSRV,
    kVertexUAV,
    kPixelCBV,
    kPixelSRV,
    kPixelUAV,

    kRootParameterIndexCount
};

class RootSignature
{
    static std::shared_ptr<RootSignature> m_instance_;

    bool m_IsValid = false;
    ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr;

public:
    static std::shared_ptr<RootSignature> Instance();
    static ID3D12RootSignature *Get();
    static bool IsValid();

    RootSignature();
};
}