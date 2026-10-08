#pragma once
#include "rendering_constants.h"
#include "Components/light.h"
#include "Rendering/CabotEngine/Graphics/Texture2DArray.h"

namespace engine
{
/// <summary>
/// Lightの一覧とShadowMapを管理し、Shaderに渡すBufferを更新するクラスです。
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
    std::shared_ptr<StructuredBuffer> m_lights_buffer_;
    std::shared_ptr<ConstantBuffer> m_light_count_buffer_;
    std::shared_ptr<DescriptorHandle> m_lights_buffer_handle_;

    // depth textures related
    ComPtr<ID3D12DescriptorHeap> m_dsv_heap_;
    std::vector<std::shared_ptr<DepthTexture>> m_shadow_maps_;
    std::shared_ptr<Texture2DArray> m_depth_textures_;
    std::shared_ptr<DescriptorHandle> m_shadow_map_handle_;
    std::set<int> m_free_depth_texture_handles_;
    std::shared_ptr<ConstantBuffer> m_cascade_slices_buffer_;

    /// <summary>
    /// Lightの数をConstantBufferに書き込みます。Bufferがない場合は作成します。
    /// </summary>
    void UpdateLightCountBuffer();
    /// <summary>
    /// 各LightのLightDataを更新し、StructuredBufferに書き込みます。Bufferがない場合は作成します。
    /// </summary>
    void UpdateLightBuffer();
    /// <summary>
    /// Lightの数のBufferを更新し、CommandListに設定します。
    /// </summary>
    void SetLightCountBuffer();
    /// <summary>
    /// LightDataのBufferを更新し、CommandListに設定します。
    /// </summary>
    void SetLightBuffer();
    /// <summary>
    /// Lightの数とLightDataのBufferを更新し、CommandListに設定します。
    /// </summary>
    void SetBuffers();

    /// <summary>
    /// ShadowMap用のTexture2DArrayとDSVを作成し、すべての要素を空きとして登録します。
    /// </summary>
    void CreateShadowMapResource();
    /// <summary>
    /// ShadowMapを深度を書き込める状態に遷移させます。
    /// </summary>
    void BeginDepthRender();
    /// <summary>
    /// ShadowMapをPixelShaderから読める状態に戻します。
    /// </summary>
    void EndDepthRender();

public:
    /// <summary>
    /// Lightingの唯一のインスタンスを取得します。
    /// </summary>
    static Lighting *Instance();

    /// <summary>
    /// ShadowMapを持つ各LightのViewProjectionMatrixを計算し、Bufferに書き込みます。
    /// </summary>
    /// <param name="view">カメラのViewMatrix</param>
    /// <param name="proj">カメラのProjectionMatrix</param>
    void UpdateLightsViewProjMatrixBuffer(const Matrix &view, const Matrix &proj);
    /// <summary>
    /// Cascadeの区切りのBufferをCommandListに設定します。Bufferがない場合は作成します。
    /// </summary>
    void SetCascadeSlicesBuffer();
    /// <summary>
    /// LightのViewProjectionMatrixのBufferをCommandListに設定します。
    /// </summary>
    void SetLightsViewProjMatrix() const;
    /// <summary>
    /// ShadowMapをCommandListに設定します。ShadowMapがない場合は作成します。
    /// </summary>
    void SetShadowMap();

    /// <summary>
    /// LightにShadowMapを割り当てます。空きが足りない場合は、待機リストに追加します。
    /// </summary>
    /// <param name="light">影を落とすLight</param>
    void TryApplyShadow(const std::shared_ptr<Light> &light);
    /// <summary>
    /// LightのShadowMapを解放します。待機中のLightがある場合は、Main Cameraに最も近いものに割り当てます。
    /// </summary>
    /// <param name="light">影を落とすのをやめるLight</param>
    void RemoveShadow(const std::shared_ptr<Light> &light);
    /// <summary>
    /// Lightを登録します。
    /// </summary>
    /// <param name="light">登録するLight</param>
    void AddLight(std::shared_ptr<Light> light);
    /// <summary>
    /// Lightを取り除き、ShadowMapを持っている場合は解放します。
    /// </summary>
    /// <param name="light">取り除くLight</param>
    void RemoveLight(const std::shared_ptr<Light> &light);

    /// <summary>
    /// Cascadeの区切りとなる距離をBufferに書き込みます。
    /// </summary>
    /// <param name="shadow_cascade_slices">各Cascadeの遠い側の距離</param>
    static void SetCascadeSlices(
        std::array<float, RenderingConstants::kShadowCascadeCount> shadow_cascade_slices);
};
}