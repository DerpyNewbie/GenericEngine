#pragma once
#include <memory>
#include <vector>

#include "Rendering/CaboRenderingSystem/Pipeline/i_render_pipeline.h"
#include "Rendering/CaboRenderingSystem/Pipeline/render_stages.h"
#include "Rendering/CaboRenderingSystem/Pipeline/shadow_system.h"
#include "Rendering/CaboRenderingSystem/RHI/gpu_resources.h"

namespace CRS
{
/// <summary>
/// Forwardパイプライン用のカメラ設定です。
/// </summary>
class ForwardCameraSettings final : public ICameraSettings
{
public:
    // MSAAのサンプル数（描画先のテクスチャも同じサンプル数で作る必要があります）
    uint32_t sample_count = 1;
    // 予約（深度プリパスはまだありません）
    bool depth_prepass = false;

    std::unique_ptr<ICameraSettings> Clone() const override
    {
        return std::make_unique<ForwardCameraSettings>(*this);
    }
};

/// <summary>
/// シーン全体で1つのパイプライン設定です。
/// </summary>
struct ForwardRenderingSettings
{
    // シャドウマップ1枚の一辺のピクセル数
    uint32_t shadow_map_size = 2048;
    // シャドウマップの最大枚数（平行光源は3枚、スポットライトは1枚使う）
    uint32_t max_shadow_maps = 10;
};

/// <summary>
/// Forwardパイプラインです。カメラごとに、影のビューを描いてから、登録されたパスを登録順で実行します。
/// </summary>
class ForwardPipeline final : public IRenderPipeline
{
    IRenderDevice &m_device_;
    ForwardRenderingSettings m_settings_;
    std::vector<std::unique_ptr<IRenderPass>> m_passes_;
    CullingStage m_culling_stage_;
    DrawListBuilder m_draw_list_builder_;
    ShadowSystem m_shadow_system_;

    // 組み込みのシェーダー（Shaders/）。標準のマテリアル、標準の影、スカイボックス、線、UIに使う
    Shader m_standard_vertex_shader_;
    Shader m_standard_pixel_shader_;
    Shader m_shadow_vertex_shader_;
    Shader m_skybox_vertex_shader_;
    Shader m_skybox_pixel_shader_;
    Shader m_line_vertex_shader_;
    Shader m_line_pixel_shader_;
    Shader m_ui_vertex_shader_;
    Shader m_ui_pixel_shader_;
    // マテリアルがテクスチャを設定していないスロットに入れる1x1の白
    Texture m_white_texture_;
    // マテリアルがサンプラーを設定していないスロットに入れる（Linear / Wrap）
    Sampler m_default_sampler_;

    // スナップショットのカメラを、priorityの小さい順にRenderViewへ変換する
    std::vector<RenderView> BuildCameraViews(const FrameSnapshot &frame) const;
    /// <summary>
    /// カメラの描画先を、ビューの設定でクリアします。以降のパスは上に重ねて描きます。
    /// </summary>
    static void ClearView(ICommandList &command_list, const RenderView &view);
    // 1つのビューについて、受け付けるパスを順に実行する（カリングと描画リストの作成もここで行う）
    void RenderSingleView(const FrameSnapshot &frame, ICommandList &command_list, const MaterialRegistry &materials,
                          const RenderView &view, const FrameLighting &lighting);

public:
    /// <summary>
    /// 組み込みのシェーダーをコンパイルし、影のテクスチャなどを作って、標準のパス（Shadow → Skybox → Forward → Line → UI）を登録します。
    /// </summary>
    ForwardPipeline(IRenderDevice &device, const ForwardRenderingSettings &settings = {});

    // ---- IRenderPipelineの実装（説明はIRenderPipelineを参照） ----
    std::unique_ptr<ICameraSettings> CreateCameraSettings() const override;
    /// <summary>
    /// 組み込みのforward_standard.hlslを使い、Forwardパス（ライティングあり）とShadowパスを持つマテリアルを作ります。
    /// </summary>
    Material CreateDefaultMaterial() const override;
    void AddPass(std::unique_ptr<IRenderPass> pass) override;
    void Render(const FrameSnapshot &frame, ICommandList &command_list, const MaterialRegistry &materials) override;

    /// <summary>
    /// 作成時に渡した設定です。
    /// </summary>
    const ForwardRenderingSettings &Settings() const
    {
        return m_settings_;
    }
};
}