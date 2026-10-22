#pragma once
#include <memory>

#include "Rendering/CaboRenderingSystem/Material/material.h"
#include "Rendering/CaboRenderingSystem/Pipeline/i_render_pass.h"
#include "Rendering/CaboRenderingSystem/RHI/i_command_list.h"
#include "Rendering/CaboRenderingSystem/Scene/camera_data_store.h"
#include "Rendering/CaboRenderingSystem/Scene/frame_snapshot.h"

namespace CRS
{
/// <summary>
/// 「何を、どの順で描くか」を決めます。起動時に決めて途中で変えません。
/// RenderEngineは知らず、渡されたスナップショットを読むだけです。
/// </summary>
class IRenderPipeline
{
public:
    virtual ~IRenderPipeline() = default;

    /// <summary>
    /// このパイプライン用のカメラ設定を作ります。カメラ作成時にRenderEngineが呼びます。
    /// </summary>
    virtual std::unique_ptr<ICameraSettings> CreateCameraSettings() const = 0;

    /// <summary>
    /// このパイプライン用の既定のマテリアルを作ります（マテリアルはパイプラインごとに存在します）。
    /// </summary>
    virtual Material CreateDefaultMaterial() const = 0;

    /// <summary>
    /// パスを登録します。登録順が実行順です。
    /// </summary>
    virtual void AddPass(std::unique_ptr<IRenderPass> pass) = 0;

    virtual void Render(const FrameSnapshot &frame, ICommandList &command_list, const MaterialRegistry &materials) = 0;
};
}