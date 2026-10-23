#pragma once
#include "Rendering/CaboRenderingSystem/Pipeline/i_render_pass.h"

namespace CRS
{
/// <summary>
/// 影のビューに、マテリアルのShadowエントリで深度だけを描くパスです。影のビュー（RenderViewKind::Shadow）だけを描きます。
/// </summary>
class ShadowPass final : public IRenderPass
{
public:
    // ---- IRenderPassの実装（説明はIRenderPassを参照） ----
    PassTag GetTag() const override;
    bool AcceptsView(const RenderView &view, const FrameSnapshot &frame) const override;
    void Execute(const PassContext &context) override;
};

/// <summary>
/// スカイボックスを描くパスです。メッシュより先に、一番奥に描きます。
/// マテリアルは使わず、SceneEnvironment::skyboxが設定されているカメラのビューだけを描きます。
/// </summary>
class SkyboxPass final : public IRenderPass
{
    ShaderHandle m_vertex_shader_;
    ShaderHandle m_pixel_shader_;

public:
    /// <summary>
    /// 使うシェーダーを受け取ります。シェーダーはForwardPipelineが持ち、このパスより長く生きます。
    /// </summary>
    SkyboxPass(ShaderHandle vertex_shader, ShaderHandle pixel_shader);

    // ---- IRenderPassの実装（説明はIRenderPassを参照） ----
    PassTag GetTag() const override;
    bool UsesDrawList() const override;
    bool AcceptsView(const RenderView &view, const FrameSnapshot &frame) const override;
    void Execute(const PassContext &context) override;
};

/// <summary>
/// 不透明・半透明のメッシュを、マテリアルのForwardエントリで描くパスです。カメラのビューを描きます。
/// </summary>
class ForwardPass final : public IRenderPass
{
public:
    // ---- IRenderPassの実装（説明はIRenderPassを参照） ----
    PassTag GetTag() const override;
    bool AcceptsView(const RenderView &view, const FrameSnapshot &frame) const override;
    void Execute(const PassContext &context) override;
};

/// <summary>
/// デバッグ表示の線（Gizmos）を描くパスです。draw_overlaysがtrueのカメラ（画面に出すカメラ）だけを描きます。
/// </summary>
class LinePass final : public IRenderPass
{
    ShaderHandle m_vertex_shader_;
    ShaderHandle m_pixel_shader_;

public:
    /// <summary>
    /// 使うシェーダーを受け取ります。シェーダーはForwardPipelineが持ち、このパスより長く生きます。
    /// </summary>
    LinePass(ShaderHandle vertex_shader, ShaderHandle pixel_shader);

    // ---- IRenderPassの実装（説明はIRenderPassを参照） ----
    PassTag GetTag() const override;
    bool UsesDrawList() const override;
    bool AcceptsView(const RenderView &view, const FrameSnapshot &frame) const override;
    void Execute(const PassContext &context) override;
};

/// <summary>
/// UI（画像、テキスト）を画面の一番手前に描くパスです。draw_overlaysがtrueのカメラだけを描きます。
/// バッチにUI用のShaderのMaterialがあればそのShaderで、なければ標準のShaderで描きます。
/// </summary>
class UiPass final : public IRenderPass
{
    ShaderHandle m_vertex_shader_;
    ShaderHandle m_pixel_shader_;

public:
    /// <summary>
    /// 使うシェーダーを受け取ります。シェーダーはForwardPipelineが持ち、このパスより長く生きます。
    /// </summary>
    UiPass(ShaderHandle vertex_shader, ShaderHandle pixel_shader);

    // ---- IRenderPassの実装（説明はIRenderPassを参照） ----
    PassTag GetTag() const override;
    bool UsesDrawList() const override;
    bool AcceptsView(const RenderView &view, const FrameSnapshot &frame) const override;
    void Execute(const PassContext &context) override;
};
}