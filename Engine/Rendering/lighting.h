#pragma once
#include "rendering_constants.h"
#include "Components/light.h"

namespace engine
{
/// <summary>
/// シーン内のLightとShadowMapを管理し、ライティング用のBufferをShaderに渡すシングルトンクラスです。
/// </summary>
class Lighting
{
    friend class RenderPipeline;
    friend class Light;

    // lighting related
    std::vector<std::shared_ptr<Light>> m_lights_;
    std::vector<std::shared_ptr<Light>> m_waiting_lights_;
    std::array<Matrix, RenderingConstants::kMaxShadowMapCount> m_light_view_proj_matrices_;
    std::shared_ptr<StructuredBuffer> m_light_view_proj_matrices_buffer_;
    std::shared_ptr<DescriptorHandle> m_light_view_proj_handle_;
    std::shared_ptr<StructuredBufferData> m_lights_buffer_;
    std::shared_ptr<DescriptorHandle> m_lights_buffer_handle_;

    // depth textures related
    ComPtr<ID3D12DescriptorHeap> m_dsv_heap_;
    std::vector<std::shared_ptr<DepthTexture>> m_shadow_maps_;
    std::shared_ptr<Texture2DArray> m_depth_textures_;
    std::shared_ptr<DescriptorHandle> m_shadow_map_handle_;
    std::set<int> m_free_depth_texture_handles_;

    /// <summary>
    /// Lightの数をグローバルな値"LightCount"として設定します。
    /// </summary>
    static void SetLightCountBuffer();
    /// <summary>
    /// 各Lightのデータを更新して、グローバルなBuffer"Lights"に書き込みます。
    /// </summary>
    void SetLightBuffer();
    /// <summary>
    /// Lightの数とLightのデータをグローバルなBufferに設定します。
    /// </summary>
    void SetBuffers();

    /// <summary>
    /// ShadowMap用のTexture2DArrayとDSVを作成し、すべての要素を空きとして登録します。
    /// </summary>
    void CreateShadowMapResource();
    /// <summary>
    /// ShadowMapを深度書き込み可能な状態に遷移させます。
    /// </summary>
    void BeginDepthRender();
    /// <summary>
    /// ShadowMapをShaderから読み込み可能な状態に遷移させます。
    /// </summary>
    void EndDepthRender();

public:
    /// <summary>
    /// Lightingの唯一のインスタンスを取得します。
    /// </summary>
    static Lighting *Instance();

    /// <summary>
    /// カメラの視錐台を元に、影を落とす各LightのViewProjection行列を計算してBufferに書き込みます。
    /// </summary>
    /// <param name="view">カメラのView行列</param>
    /// <param name="proj">カメラのProjection行列</param>
    void UpdateLightsViewProjMatrixBuffer(const Matrix &view, const Matrix &proj);
    /// <summary>
    /// LightのViewProjection行列のBufferをRootSignatureに設定します。
    /// </summary>
    void SetLightsViewProjMatrix() const;
    /// <summary>
    /// ShadowMapをRootSignatureに設定します。ShadowMapが未作成の場合は作成します。
    /// </summary>
    void SetShadowMap();

    /// <summary>
    /// 登録されているLightの数を取得します。
    /// </summary>
    uint32_t GetLightCount() const;

    /// <summary>
    /// LightにShadowMapを割り当てます。空きが足りない場合は影を無効にして待機リストに追加します。
    /// </summary>
    /// <param name="light">対象のLight</param>
    void TryApplyShadow(const std::shared_ptr<Light> &light);
    /// <summary>
    /// LightのShadowMapを解放します。待機中のLightがあれば、カメラに最も近いLightに割り当てます。
    /// </summary>
    /// <param name="light">対象のLight</param>
    void RemoveShadow(const std::shared_ptr<Light> &light);
    /// <summary>
    /// Lightを登録します。
    /// </summary>
    /// <param name="light">登録するLight</param>
    void AddLight(std::shared_ptr<Light> light);
    /// <summary>
    /// Lightの登録を解除し、割り当てられているShadowMapを解放します。
    /// </summary>
    /// <param name="light">解除するLight</param>
    void RemoveLight(const std::shared_ptr<Light> &light);

    /// <summary>
    /// カスケードShadowMapの分割位置をグローバルな値"ShadowCascadeSlices"として設定します。
    /// </summary>
    /// <param name="shadow_cascade_slices">各カスケードの分割位置</param>
    static void SetCascadeSlices(
        std::array<float, RenderingConstants::kShadowCascadeCount> shadow_cascade_slices);
};
}