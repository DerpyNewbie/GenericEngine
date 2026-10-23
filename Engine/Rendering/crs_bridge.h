#pragma once
#include "event.h"
#include "Asset/asset_ptr.h"
#include "Rendering/font_data.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"
#include "Rendering/CaboRenderingSystem/Scene/camera_data_store.h"
#include "Rendering/CaboRenderingSystem/Scene/mesh_renderer_data_store.h"

namespace CRS
{
class RenderEngine;
class D3D12RenderDevice;
}

namespace engine
{
class CameraComponent;

/// <summary>
/// エンジンからCaboRenderingSystem(CRS)を使うための窓口です。
/// CRSの生成と1フレームの描画（BeginFrame → Render → EndFrame）を進め、
/// ComponentやGizmos、LightをCRSに登録します。GPUのリソースは各Asset(Mesh、Texture2D、Material)が自分で持ちます。
/// </summary>
class CrsBridge
{
    /// <summary>
    /// CRSでの描画の直前に呼びます。Gizmosの線、Light、スカイボックスなどを渡し、Main Cameraが無い場合は既定の視点で描くようにします。
    /// </summary>
    static void PrepareFrame();
    /// <summary>
    /// バックバッファを描画先にして、on_overlay_renderを呼びます（EditorのImGuiなど）。
    /// </summary>
    static void RenderOverlay();

public:
    /// <summary>
    /// シーンの描画が終わった後、バックバッファを描画先にし、CRSのDescriptorHeapを設定した状態で呼ばれます。
    /// Device().NativeCommandList()に直接記録します（EditorのImGuiなど）。
    /// </summary>
    inline static Event<> on_overlay_render;

    /// <summary>
    /// CRS（D3D12デバイスとForwardパイプライン）を作成します。
    /// </summary>
    /// <returns>初期化に成功した場合はtrue</returns>
    static bool Init(HWND hwnd, uint32_t width, uint32_t height);
    /// <summary>
    /// フレームの描画を開始します。この後にComponentがCRSに描画データを登録します。
    /// </summary>
    static void BeginFrame();
    /// <summary>
    /// 登録された描画データ（メッシュ、カメラ、ライト、Gizmos、UI）をCRSで描画し、最後にon_overlay_renderを呼びます。
    /// </summary>
    static void Render();
    /// <summary>
    /// フレームの描画を終了し、画面に表示します。
    /// </summary>
    static void EndFrame();
    /// <summary>
    /// GPUの処理がすべて完了するまで待機します。
    /// </summary>
    static void WaitIdle();

    /// <summary>
    /// CRSのRenderEngineを取得します（ストア、マテリアル、デバイスなど）。
    /// </summary>
    static CRS::RenderEngine &Crs();
    /// <summary>
    /// CRSのD3D12デバイスを取得します。外部のD3D12ライブラリ（ImGuiなど）と連携する場合に使います。
    /// </summary>
    static CRS::D3D12RenderDevice &Device();

    /// <summary>
    /// Meshの描画データをCRSに登録、または更新します。idが無効な場合は新しく登録します。
    /// </summary>
    /// <param name="id">登録済みのID。新しく登録した場合は書き換えます</param>
    /// <param name="mesh">描画するMesh</param>
    /// <param name="materials">1つ目はMesh本体、以降はSubMeshに対応するMaterial</param>
    /// <param name="world">WorldMatrix</param>
    /// <param name="local_bounds">worldを基準としたbounds（カリングに使います）</param>
    /// <param name="bone_buffer">スキニングに使うボーン行列のBuffer。使わない場合は無効なハンドル</param>
    static void SubmitMeshRenderer(CRS::MeshRendererId &id, const std::shared_ptr<Mesh> &mesh,
                                   const std::vector<AssetPtr<Material>> &materials, const Matrix &world,
                                   const DirectX::BoundingBox &local_bounds, CRS::BufferHandle bone_buffer = {});
    /// <summary>
    /// Meshの描画データをCRSから取り除きます。
    /// </summary>
    static void RemoveMeshRenderer(CRS::MeshRendererId &id);

    /// <summary>
    /// Cameraの視点と描画先をCRSに登録、または更新します。Main Cameraは画面に、それ以外はRenderTextureに描きます。
    /// </summary>
    static void SubmitCamera(CRS::CameraId &id, const CameraComponent &camera);
    /// <summary>
    /// CameraをCRSから取り除きます。
    /// </summary>
    static void RemoveCamera(CRS::CameraId &id);

    /// <summary>
    /// 四角形をUIとして描きます（このフレームのみ）。MaterialのShaderがUI用ならそのShaderで、そうでなければ_MainTexを貼って描きます。
    /// </summary>
    /// <param name="screen_matrix">(-1..1)の四角形を画面の正規化座標に置くMatrix</param>
    /// <param name="material">使用するMaterial</param>
    static void AddUiImage(const Matrix &screen_matrix, const std::shared_ptr<Material> &material);
    /// <summary>
    /// 文字列をUIとして描きます（このフレームのみ）。
    /// </summary>
    /// <param name="font">使用するFont</param>
    /// <param name="position">左上の位置（画面のピクセル座標）</param>
    /// <param name="string">文字列</param>
    /// <param name="color">文字色</param>
    static void AddUiText(const std::shared_ptr<FontData> &font, Vector2 position, const std::string &string,
                          const Color &color);
};
}
