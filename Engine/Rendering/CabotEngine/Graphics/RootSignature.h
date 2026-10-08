#pragma once

namespace engine
{
/// <summary>
/// RootSignatureのParameterのindexです。
/// </summary>
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

/// <summary>
/// DirectXのRootSignatureを保持するクラスです。
/// </summary>
class RootSignature
{
    bool m_is_valid_ = false;
    ComPtr<ID3D12RootSignature> m_root_signature_ = nullptr;

public:
    constexpr static int kPreDefinedVariableCount = kMaterialCBV;
    static std::shared_ptr<RootSignature> Instance();
    /// <summary>
    /// DirectXのRootSignatureを取得します。
    /// </summary>
    static ID3D12RootSignature *Get();
    /// <summary>
    /// RootSignatureが作成済みであるかどうかを取得します。
    /// </summary>
    static bool IsValid();

    /// <summary>
    /// Engineが定義するparameter(WorldMatrix、ViewProjection、SceneData、Light、ShadowMapなど)と、Material用のCBV / SRV / UAVのTable、Samplerを持つRootSignatureを作成します。
    /// </summary>
    RootSignature();
};
}