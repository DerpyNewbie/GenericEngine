#pragma once
#include <memory>

#include "Rendering/CaboRenderingSystem/Material/material.h"
#include "Rendering/CaboRenderingSystem/Pipeline/i_render_pipeline.h"
#include "Rendering/CaboRenderingSystem/RHI/gpu_resources.h"
#include "Rendering/CaboRenderingSystem/RHI/i_render_device.h"
#include "Rendering/CaboRenderingSystem/Scene/camera_data_store.h"
#include "Rendering/CaboRenderingSystem/Scene/immediate_draw_store.h"
#include "Rendering/CaboRenderingSystem/Scene/light_data_store.h"
#include "Rendering/CaboRenderingSystem/Scene/mesh_renderer_data_store.h"
#include "Rendering/CaboRenderingSystem/Scene/scene_environment.h"

namespace CRS
{
/// <summary>
/// CRSの最上位です。デバイスとパイプラインを受け取って組み立て、1フレームの流れを制御します。
/// 全体と具象型を知るのはここだけです。
/// </summary>
class RenderEngine
{
    // デバイスはすべてのラッパーより後に破棄するので、最初に宣言する
    std::unique_ptr<IRenderDevice> m_device_;

    MeshRendererDataStore m_meshes_;
    CameraDataStore m_cameras_;
    LightDataStore m_lights_;
    SceneEnvironment m_environment_;
    ImmediateDrawStore m_immediate_;
    MaterialRegistry m_materials_;
    std::unique_ptr<IRenderPipeline> m_pipeline_;

    Texture m_main_depth_;
    uint32_t m_width_ = 0;
    uint32_t m_height_ = 0;

    ICommandList *m_command_list_ = nullptr;

    void CreateMainDepth();

public:
    RenderEngine(std::unique_ptr<IRenderDevice> device, std::unique_ptr<IRenderPipeline> pipeline, uint32_t width,
                 uint32_t height);
    ~RenderEngine();

    RenderEngine(const RenderEngine &) = delete;
    RenderEngine &operator=(const RenderEngine &) = delete;

    IRenderDevice &Device()
    {
        return *m_device_;
    }

    IRenderPipeline &Pipeline()
    {
        return *m_pipeline_;
    }

    MeshRendererDataStore &Meshes()
    {
        return m_meshes_;
    }

    CameraDataStore &Cameras()
    {
        return m_cameras_;
    }

    LightDataStore &Lights()
    {
        return m_lights_;
    }

    SceneEnvironment &Environment()
    {
        return m_environment_;
    }

    /// <summary>
    /// 毎フレーム作り直す描画（デバッグ表示の線、UI）です。EndFrameで空になります。
    /// </summary>
    ImmediateDrawStore &Immediate()
    {
        return m_immediate_;
    }

    MaterialRegistry &Materials()
    {
        return m_materials_;
    }

    /// <summary>
    /// カメラを作ります。今のパイプライン用の設定を持たせます。
    /// </summary>
    CameraId CreateCamera();
    void DestroyCamera(CameraId id);

    /// <summary>
    /// 今のパイプライン用の既定のマテリアルを作って登録します。
    /// </summary>
    MaterialId CreateDefaultMaterial();

    /// <summary>
    /// バックバッファと同じ大きさの、メインの深度バッファです。
    /// </summary>
    TextureHandle MainDepth() const
    {
        return m_main_depth_.GetHandle();
    }

    uint32_t Width() const
    {
        return m_width_;
    }

    uint32_t Height() const
    {
        return m_height_;
    }

    void Resize(uint32_t width, uint32_t height);

    // ---- 1フレームの流れ ----
    // BeginFrame → Render → EndFrame の順に呼びます。RenderFrameはそれをまとめて行います
    void BeginFrame();
    /// <summary>
    /// 現在のフレームのコマンドリストです。BeginFrameとEndFrameの間だけ有効です。
    /// </summary>
    ICommandList &CommandList();
    void Render(float time, float delta_time);
    void EndFrame();
    void RenderFrame(float time, float delta_time);
};

/// <summary>
/// D3D12デバイスとForwardパイプラインで組み立てたRenderEngineを作ります。失敗した場合はnullptrを返します。
/// </summary>
std::unique_ptr<RenderEngine> CreateDefaultRenderEngine(void *native_window, uint32_t width, uint32_t height);
}
