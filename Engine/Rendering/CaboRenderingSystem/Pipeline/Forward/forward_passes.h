#pragma once
#include "Rendering/CaboRenderingSystem/Pipeline/i_render_pass.h"

namespace CRS
{
/// <summary>
/// 影のビューに、マテリアルのShadowエントリで深度だけを描くパスです。
/// </summary>
class ShadowPass final : public IRenderPass
{
public:
    PassTag GetTag() const override;
    bool AcceptsView(const RenderView &view, const FrameSnapshot &frame) const override;
    void Execute(const PassContext &context) override;
};

/// <summary>
/// スカイボックスを描くパスです。メッシュより先に、一番奥に描きます。
/// </summary>
class SkyboxPass final : public IRenderPass
{
    ShaderHandle m_vertex_shader_;
    ShaderHandle m_pixel_shader_;

public:
    SkyboxPass(ShaderHandle vertex_shader, ShaderHandle pixel_shader);

    PassTag GetTag() const override;
    bool UsesDrawList() const override;
    bool AcceptsView(const RenderView &view, const FrameSnapshot &frame) const override;
    void Execute(const PassContext &context) override;
};

/// <summary>
/// 不透明・半透明のメッシュを、マテリアルのForwardエントリで描くパスです。
/// </summary>
class ForwardPass final : public IRenderPass
{
public:
    PassTag GetTag() const override;
    bool AcceptsView(const RenderView &view, const FrameSnapshot &frame) const override;
    void Execute(const PassContext &context) override;
};

/// <summary>
/// デバッグ表示の線（Gizmos）を描くパスです。
/// </summary>
class LinePass final : public IRenderPass
{
    ShaderHandle m_vertex_shader_;
    ShaderHandle m_pixel_shader_;

public:
    LinePass(ShaderHandle vertex_shader, ShaderHandle pixel_shader);

    PassTag GetTag() const override;
    bool UsesDrawList() const override;
    bool AcceptsView(const RenderView &view, const FrameSnapshot &frame) const override;
    void Execute(const PassContext &context) override;
};

/// <summary>
/// UI（画像、テキスト）を画面の一番手前に描くパスです。
/// </summary>
class UiPass final : public IRenderPass
{
    ShaderHandle m_vertex_shader_;
    ShaderHandle m_pixel_shader_;

public:
    UiPass(ShaderHandle vertex_shader, ShaderHandle pixel_shader);

    PassTag GetTag() const override;
    bool UsesDrawList() const override;
    bool AcceptsView(const RenderView &view, const FrameSnapshot &frame) const override;
    void Execute(const PassContext &context) override;
};
}