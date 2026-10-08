#pragma once
#include "camera.h"
#include "event.h"
#include "material.h"
#include "object_pool.h"
#include "render_command.h"
#include "render_texture.h"
#include "view_projection.h"
#include "CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
struct CameraProperty;
class Renderer;
class DepthTexture;

/// <summary>
/// CameraごとにRendererを集めて、影や各Objectの描画を行うクラスです。
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

    std::shared_ptr<Shader> m_depth_shader_;

    std::vector<RenderCommand> m_commands_;
    std::vector<std::shared_ptr<Renderer>> m_renderers_;
    std::shared_ptr<ConstantBuffer> m_scene_data_buffer_;

    Camera m_current_camera_;
    std::vector<Camera> m_requesting_cameras_;
    uint32_t m_current_view_proj_matrix_index_;
    std::array<ObjectPool<std::shared_ptr<ConstantBuffer>>, RenderEngine::kFrame_Buffer_Count> m_view_proj_matrix_buffers_
        = {ObjectPool(0, kOnViewProjBuffCreate), ObjectPool(0, kOnViewProjBuffCreate)};


    /// <summary>
    /// 描画を要求されたすべてのCameraと、Main Camera(ない場合は既定の視点)で描画を行い、on_renderingを呼び出します。
    /// </summary>
    void InvokeDrawCall();

    /// <summary>
    /// Main Cameraの視点でBackBufferに描画します。描画の間だけ、AspectRatioをWindowに合わせます。
    /// </summary>
    /// <param name="main_camera">Main Camera</param>
    void RenderMainRenderTarget(const std::shared_ptr<CameraComponent> &main_camera);
    /// <summary>
    /// Cameraに設定されているRenderTextureとDepthTextureに描画します。どちらもない場合は何もしません。
    /// </summary>
    /// <param name="camera">描画するCamera</param>
    void RenderCamera(const Camera &camera);
    /// <summary>
    /// Main Cameraがない場合に、既定の視点でBackBufferに描画します。
    /// </summary>
    void RenderVoid();
    /// <summary>
    /// 各Bufferを設定してSkyboxを描画した後、RenderCommandを並び替えて実行し、最後にGizmosを描画します。
    /// </summary>
    /// <param name="view">ViewMatrix</param>
    /// <param name="proj">ProjectionMatrix</param>
    void Render(const Matrix &view, const Matrix &proj);

    /// <summary>
    /// 現在描画中のCameraを設定します。
    /// </summary>
    void SetCurrentCamera(const Camera &camera);
    /// <summary>
    /// 画面のサイズ、ShadowMapのサイズ、時間をBufferに書き込み、CommandListに設定します。
    /// </summary>
    void SetSceneData();
    /// <summary>
    /// ViewMatrixとProjectionMatrixをBufferに書き込み、CommandListに設定します。
    /// </summary>
    void SetViewProjMatrix(const Matrix &view, const Matrix &proj);
    /// <summary>
    /// ViewProjection、SceneData、Lighting関連のBufferをCommandListに設定し、Skyboxを描画します。
    /// </summary>
    void UpdateBuffer(const Matrix &view, const Matrix &proj);
    /// <summary>
    /// すべてのRendererをShadowMapに描画します。Lightが1つもない場合は何もしません。
    /// </summary>
    void DepthRender();
    /// <summary>
    /// 登録されているRenderCommandを順番に実行し、リストをクリアします。
    /// </summary>
    void ExecuteRenderCommands();

public:
    Event<> on_rendering;

    /// <summary>
    /// ViewProjection用のBufferのPoolの最大数を設定します。
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
    /// 現在描画中のCameraを取得します。
    /// </summary>
    static Camera GetCurrentCamera();
    /// <summary>
    /// RenderQueue、カメラからの距離、Shaderから、描画順を決めるキーを生成します。不透明なものは手前から、Blendを使うものは奥から描画される順になります。
    /// </summary>
    static uint64_t GenerateSortKey(uint64_t render_queue, float depth, const Shader &shader);

    /// <summary>
    /// Meshの描画を、Materialごとに1つのRenderCommandとして登録します。2つ目以降のMaterialはSubMeshに対応します。
    /// </summary>
    /// <param name="mesh">描画するMesh</param>
    /// <param name="materials">使用するMaterialのリスト</param>
    /// <param name="pos">並び替えに使うWorld空間での位置</param>
    /// <param name="world_matrix_address">WorldMatrixのBufferのアドレス</param>
    /// <param name="bone_matrices_handle">BoneのMatrixのBufferのHandle</param>
    static void Submit(const std::shared_ptr<Mesh> &mesh, std::vector<AssetPtr<Material>> &materials, Vector3 pos, D3D12_GPU_VIRTUAL_ADDRESS world_matrix_address = {}, D3D12_GPU_DESCRIPTOR_HANDLE bone_matrices_handle = {});
    /// <summary>
    /// 文字列の描画をRenderCommandとして登録します。
    /// </summary>
    /// <param name="font_data">使用するフォント</param>
    /// <param name="position">描画する位置</param>
    /// <param name="string">描画する文字列</param>
    /// <param name="color">文字の色</param>
    static void Submit(AssetPtr<FontData> font_data, Vector2 position, const std::string &string, Color color);
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
    /// 指定されたCameraでの描画を要求します。InvokeDrawCallで描画されます。
    /// </summary>
    /// <param name="camera">描画するCamera</param>
    static void RequestRender(Camera camera);
};
}