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
/// <para>ゲーム側は各ストア（Meshes、Cameras、Lightsなど）に登録・更新し、Renderの中でそのスナップショットを描きます。</para>
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

    // バックバッファと同じ大きさの深度バッファ（画面に描くカメラが使う）
    Texture m_main_depth_;
    uint32_t m_width_ = 0;
    uint32_t m_height_ = 0;

    // BeginFrameからEndFrameの間だけ有効
    ICommandList *m_command_list_ = nullptr;

    // 今の大きさでメインの深度バッファを作り直す
    void CreateMainDepth();

public:
    /// <summary>
    /// デバイスとパイプラインを受け取り、メインの深度バッファを作ります。
    /// </summary>
    RenderEngine(std::unique_ptr<IRenderDevice> device, std::unique_ptr<IRenderPipeline> pipeline, uint32_t width,
                 uint32_t height);
    /// <summary>
    /// GPUの処理がすべて終わるまで待ってから破棄します。
    /// </summary>
    ~RenderEngine();

    RenderEngine(const RenderEngine &) = delete;
    RenderEngine &operator=(const RenderEngine &) = delete;

    /// <summary>
    /// GPUリソースを作るためのデバイスです。
    /// </summary>
    IRenderDevice &Device()
    {
        return *m_device_;
    }

    /// <summary>
    /// 描画の手順を決めるパイプラインです。
    /// </summary>
    IRenderPipeline &Pipeline()
    {
        return *m_pipeline_;
    }

    /// <summary>
    /// 描くメッシュの登録先です。
    /// </summary>
    MeshRendererDataStore &Meshes()
    {
        return m_meshes_;
    }

    /// <summary>
    /// カメラの登録先です。カメラはCreateCameraで作ってください（パイプライン用の設定を持たせるため）。
    /// </summary>
    CameraDataStore &Cameras()
    {
        return m_cameras_;
    }

    /// <summary>
    /// ライトの登録先です。
    /// </summary>
    LightDataStore &Lights()
    {
        return m_lights_;
    }

    /// <summary>
    /// シーン全体の描画設定（スカイボックス、環境光、影の分割）です。
    /// </summary>
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

    /// <summary>
    /// マテリアルの登録先です。メッシュのDrawRangeはここに登録したMaterialIdを指します。
    /// </summary>
    MaterialRegistry &Materials()
    {
        return m_materials_;
    }

    /// <summary>
    /// カメラを作ります。今のパイプライン用の設定を持たせます。
    /// </summary>
    CameraId CreateCamera();
    /// <summary>
    /// カメラを取り除きます。
    /// </summary>
    void DestroyCamera(CameraId id);

    /// <summary>
    /// 今のパイプライン用の既定のマテリアルを作って登録します。
    /// </summary>
    MaterialId CreateDefaultMaterial();

    /// <summary>
    /// バックバッファと同じ大きさの、メインの深度バッファです。Resizeで作り直すので、毎フレーム取得してください。
    /// </summary>
    TextureHandle MainDepth() const
    {
        return m_main_depth_.GetHandle();
    }

    /// <summary>
    /// バックバッファの幅（ピクセル）です。
    /// </summary>
    uint32_t Width() const
    {
        return m_width_;
    }

    /// <summary>
    /// バックバッファの高さ（ピクセル）です。
    /// </summary>
    uint32_t Height() const
    {
        return m_height_;
    }

    /// <summary>
    /// バックバッファとメインの深度バッファを作り直します。幅か高さが0なら何もしません（最小化中など）。
    /// </summary>
    void Resize(uint32_t width, uint32_t height);

    // ---- 1フレームの流れ ----
    // BeginFrame → Render → EndFrame の順に呼びます。RenderFrameはそれをまとめて行います
    /// <summary>
    /// フレームを始め、このフレームのコマンドリストを用意します。
    /// </summary>
    void BeginFrame();
    /// <summary>
    /// 現在のフレームのコマンドリストです。BeginFrameとEndFrameの間だけ有効です。
    /// </summary>
    ICommandList &CommandList();
    /// <summary>
    /// 各ストアのスナップショットを取り、パイプラインで描きます。
    /// </summary>
    /// <param name="time">起動からの秒数（シェーダーのtime.x）</param>
    /// <param name="delta_time">前フレームからの秒数（シェーダーのtime.y）</param>
    void Render(float time, float delta_time);
    /// <summary>
    /// コマンドリストをGPUに送って画面に表示し、Immediate()を空にします。
    /// </summary>
    void EndFrame();
    /// <summary>
    /// BeginFrame、Render、EndFrameをまとめて呼びます（間に自分で記録するものがない場合）。
    /// </summary>
    void RenderFrame(float time, float delta_time);
};

/// <summary>
/// D3D12デバイスとForwardパイプラインで組み立てたRenderEngineを作ります。失敗した場合はnullptrを返します。
/// パスは標準の5つ（Shadow、Skybox、Forward、Line、UI）が登録済みです。
/// </summary>
std::unique_ptr<RenderEngine> CreateDefaultRenderEngine(void *native_window, uint32_t width, uint32_t height);
}
