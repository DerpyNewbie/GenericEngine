#pragma once
#include "camera.h"
#include "event.h"
#include "material.h"
#include "object_pool.h"
#include "render_command.h"
#include "render_texture.h"
#include "view_projection.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"

namespace engine
{
class GpuResourceGroup;
struct CameraProperty;
class Renderer;
class DepthTexture;

/// <summary>
/// Rendererから受け取った描画コマンドを、カメラごとにShadowMapの描画、ソート、実行まで行う描画パイプラインです。
/// </summary>
class RenderPipeline
{
    friend class Light;
    friend class Engine;
    friend class CameraComponent;

    constexpr static size_t kStableCameraCount = 8;
    inline const static std::function<std::shared_ptr<ConstantBuffer>()> kOnViewProjBuffCreate = [] {
        auto view_proj_buff = std::make_shared<ConstantBuffer>(sizeof(ViewProjection));
        view_proj_buff->CreateBuffer();
        return view_proj_buff;
    };

    uint16_t m_effect_render_queue_;

    std::shared_ptr<Shader> m_depth_shader_;

    std::vector<RenderCommand> m_render_commands_;
    std::vector<std::shared_ptr<Renderer>> m_renderers_;
    std::shared_ptr<ConstantBufferData> m_scene_data_buffer_data_;

    Camera m_current_camera_;
    std::vector<Camera> m_requesting_cameras_;
    uint32_t m_current_view_proj_matrix_index_;
    ObjectPool<std::shared_ptr<ConstantBuffer>> m_view_proj_matrix_buffers_ = ObjectPool(0, kOnViewProjBuffCreate);

    std::unordered_map<std::shared_ptr<MaterialBlock>, std::shared_ptr<GpuResourceGroup>> m_material_block_gpu_resource_groups_map_;
    /// <summary>
    /// 1フレーム分の描画を行います。要求されたカメラのRenderTextureへの描画を行った後、メインカメラ(存在しない場合はデフォルトの視点)で画面に描画します。
    /// </summary>
    void InvokeDrawCall();

    /// <summary>
    /// メインカメラのアスペクト比をWindowに合わせ、ShadowMapの描画後に画面へ描画します。
    /// </summary>
    /// <param name="main_camera">メインカメラ</param>
    void RenderMainRenderTarget(const std::shared_ptr<CameraComponent> &main_camera);
    /// <summary>
    /// カメラに設定されたRenderTexture/DepthTextureへ描画します。どちらも設定されていない場合は何もしません。
    /// </summary>
    /// <param name="camera">描画に使用するカメラ</param>
    void RenderCamera(const Camera &camera);
    /// <summary>
    /// メインカメラが存在しない場合に、原点から前方を向いたデフォルトの視点で画面に描画します。
    /// </summary>
    void RenderVoid();
    /// <summary>
    /// 各Bufferを更新し、描画コマンドをソートして実行した後、Gizmosを描画します。
    /// </summary>
    /// <param name="view">View行列</param>
    /// <param name="proj">Projection行列</param>
    void Render(const Matrix &view, const Matrix &proj);

    /// <summary>
    /// 現在描画中のカメラを設定します。
    /// </summary>
    /// <param name="camera">設定するカメラ</param>
    void SetCurrentCamera(const Camera &camera);
    /// <summary>
    /// 画面サイズ、時間、カメラの位置と向きなどをグローバルなBuffer"SceneData"に設定します。
    /// </summary>
    void SetSceneData();
    /// <summary>
    /// View/Projection行列をBufferに書き込み、RootSignatureに設定します。
    /// </summary>
    /// <param name="view">View行列</param>
    /// <param name="proj">Projection行列</param>
    void SetViewProjMatrix(const Matrix &view, const Matrix &proj);
    /// <summary>
    /// View/Projection行列、SceneData、ライティング関連のBufferを設定し、Skyboxを描画します。
    /// </summary>
    /// <param name="view">View行列</param>
    /// <param name="proj">Projection行列</param>
    void UpdateBuffer(const Matrix &view, const Matrix &proj);
    /// <summary>
    /// Lightが存在する場合、すべてのRendererの深度をShadowMapに描画します。
    /// </summary>
    void DepthRender();

    /// <summary>
    /// ソート済みの描画コマンドを順に実行します。Shader、Mesh、Materialが変わった時のみ設定し直し、テキストはSpriteBatchで描画します。
    /// </summary>
    void ExecuteRenderCommands();

public:
    Event<> on_cmd_list_open;
    Event<> on_rendering;

    /// <summary>
    /// カメラ用のView/Projection行列Bufferのプールの最大数を設定します。
    /// </summary>
    static void Init();
    /// <summary>
    /// RenderPipelineの唯一のインスタンスを取得します。
    /// </summary>
    static RenderPipeline *Instance();
    /// <summary>
    /// 登録されているRendererの数を取得します。
    /// </summary>
    static size_t GetRendererCount();
    /// <summary>
    /// 現在描画中のカメラを取得します。
    /// </summary>
    static Camera GetCurrentCamera();
    /// <summary>
    /// 描画コマンドのソートキーを生成します。不透明な場合はShader、手前からの順に、透明な場合は奥からの順に並ぶようにします。
    /// </summary>
    /// <param name="render_queue">RenderQueue。最優先でソートされます</param>
    /// <param name="depth">カメラからの距離</param>
    /// <param name="shader">使用するShader</param>
    static uint64_t GenerateSortKey(uint64_t render_queue, float depth, const Shader &shader);

    /// <summary>
    /// Meshの描画コマンドをMaterialごとに登録します。1つ目のMaterialはMesh全体、以降はSubMeshに対応します。
    /// </summary>
    /// <param name="mesh">描画するMesh</param>
    /// <param name="materials">使用するMaterial</param>
    /// <param name="pos">ソートに使用する位置</param>
    /// <param name="world_matrix_address">WorldMatrixのBufferのアドレス</param>
    /// <param name="bone_matrices_handle">ボーン行列のBufferのハンドル</param>
    static void Submit(const std::shared_ptr<Mesh> &mesh, const std::vector<AssetPtr<Material>> &materials, Vector3 pos, D3D12_GPU_VIRTUAL_ADDRESS world_matrix_address = {}, D3D12_GPU_DESCRIPTOR_HANDLE bone_matrices_handle = {});
    /// <summary>
    /// テキストの描画コマンドを登録します。
    /// </summary>
    /// <param name="font_data">使用するFont</param>
    /// <param name="position">描画位置</param>
    /// <param name="string">描画する文字列</param>
    /// <param name="color">文字色</param>
    static void Submit(const AssetPtr<FontData> &font_data, Vector2 position, const std::string &string, Color color);
    /// <summary>
    /// Rendererを登録します。
    /// </summary>
    /// <param name="renderer">登録するRenderer</param>
    static void AddRenderer(std::shared_ptr<Renderer> renderer);
    /// <summary>
    /// Rendererの登録を解除します。
    /// </summary>
    /// <param name="renderer">解除するRenderer</param>
    static void RemoveRenderer(const std::shared_ptr<Renderer> &renderer);
    /// <summary>
    /// このフレームでカメラのRenderTextureへの描画を要求します。
    /// </summary>
    /// <param name="camera">描画に使用するカメラ</param>
    static void RequestRender(Camera camera);
};
}