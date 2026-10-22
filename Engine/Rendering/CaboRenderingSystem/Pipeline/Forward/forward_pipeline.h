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
    uint32_t sample_count = 1;
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
    uint32_t shadow_map_size = 2048;
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

    Shader m_standard_vertex_shader_;
    Shader m_standard_pixel_shader_;
    Shader m_shadow_vertex_shader_;
    Shader m_skybox_vertex_shader_;
    Shader m_skybox_pixel_shader_;
    Shader m_line_vertex_shader_;
    Shader m_line_pixel_shader_;
    Shader m_ui_vertex_shader_;
    Shader m_ui_pixel_shader_;
    Texture m_white_texture_;
    Sampler m_default_sampler_;

    std::vector<RenderView> BuildCameraViews(const FrameSnapshot &frame) const;
    /// <summary>
    /// カメラの描画先を、ビューの設定でクリアします。以降のパスは上に重ねて描きます。
    /// </summary>
    static void ClearView(ICommandList &command_list, const RenderView &view);
    void RenderSingleView(const FrameSnapshot &frame, ICommandList &command_list, const MaterialRegistry &materials,
                          const RenderView &view, const FrameLighting &lighting);

public:
    ForwardPipeline(IRenderDevice &device, const ForwardRenderingSettings &settings = {});

    std::unique_ptr<ICameraSettings> CreateCameraSettings() const override;
    Material CreateDefaultMaterial() const override;
    void AddPass(std::unique_ptr<IRenderPass> pass) override;
    void Render(const FrameSnapshot &frame, ICommandList &command_list, const MaterialRegistry &materials) override;

    const ForwardRenderingSettings &Settings() const
    {
        return m_settings_;
    }
};
}