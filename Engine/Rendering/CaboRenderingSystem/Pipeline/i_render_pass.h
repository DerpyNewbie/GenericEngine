#pragma once
#include "Rendering/CaboRenderingSystem/Material/material.h"
#include "Rendering/CaboRenderingSystem/Pipeline/render_stages.h"
#include "Rendering/CaboRenderingSystem/Pipeline/render_view.h"
#include "Rendering/CaboRenderingSystem/RHI/i_command_list.h"
#include "Rendering/CaboRenderingSystem/RHI/i_render_device.h"
#include "Rendering/CaboRenderingSystem/Scene/frame_snapshot.h"

namespace CRS
{
/// <summary>
/// パスの実行に必要なものです。
/// </summary>
struct PassContext
{
    ICommandList &command_list;
    IRenderDevice &device;
    const RenderView &view;
    const FrameSnapshot &frame;
    const DrawList &draw_list;
    const MaterialRegistry &materials;
    const ViewConstants &view_constants;
    const FrameLighting &lighting;
    // マテリアルがテクスチャ / サンプラーを指定していないスロットに使う既定値
    TextureHandle default_texture;
    SamplerHandle default_sampler;
};

/// <summary>
/// 1つの描画パスです。パイプラインにAddPassで登録します。
/// 描画先のクリアはパイプラインが済ませているので、パスは上に重ねて描きます。
/// </summary>
class IRenderPass
{
public:
    virtual ~IRenderPass() = default;

    /// <summary>
    /// このパスが描くマテリアルのエントリです。マテリアルを使わないパスは、描画リストを使いません。
    /// </summary>
    virtual PassTag GetTag() const = 0;
    /// <summary>
    /// マテリアルを使って描画リストを作るかどうかです。
    /// </summary>
    virtual bool UsesDrawList() const
    {
        return true;
    }
    /// <summary>
    /// そのビューを描くかどうかです。
    /// </summary>
    virtual bool AcceptsView(const RenderView &view, const FrameSnapshot &frame) const = 0;
    virtual void Execute(const PassContext &context) = 0;
};

/// <summary>
/// メッシュを1つ描くためのバインドを作ります。ShadowとForwardで共有します。
/// </summary>
Bindings MakeMeshBindings(const PassContext &context, const DrawItem &item, ObjectConstants &object_constants);
}