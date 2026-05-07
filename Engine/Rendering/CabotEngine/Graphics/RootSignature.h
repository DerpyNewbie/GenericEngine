#pragma once

namespace engine
{
enum kRootParameterIndex
{
    kWorldCBV,
    kViewProjCBV,
    kSceneDataCBV,
    kCascadeSlicesCBV,
    kLightCountCBV,
    kBoneSRV,
    kLightSRV,
    kLightViewProj,
    kShadowMapSRV,

    //エンジン定義のルートパラメータはここより上に追加してください
    kMaterialCBV,
    kMaterialSRV,
    kMaterialUAV,

    kRootParameterIndexCount
};

class RootSignature
{
    bool m_is_valid_ = false;
    ComPtr<ID3D12RootSignature> m_root_signature_ = nullptr;

public:
    constexpr static int kPreDefinedVariableCount = kMaterialCBV;

    static std::shared_ptr<RootSignature> Instance();
    static ID3D12RootSignature *Get();
    static bool IsValid();

    RootSignature();
};
}