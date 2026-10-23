#pragma once
#include <array>
#include <memory>
#include <vector>

#include "Rendering/CaboRenderingSystem/Core/math_types.h"
#include "Rendering/CaboRenderingSystem/Core/slot_pool.h"
#include "Rendering/CaboRenderingSystem/RHI/rhi_types.h"

namespace CRS
{
// CameraDataStoreに登録したカメラを指すID
using CameraId = Handle<struct CameraTag>;

/// <summary>
/// パイプライン固有のカメラ設定です。パイプラインが生成するので、受け側はキャストだけで使えます。
/// </summary>
class ICameraSettings
{
public:
    virtual ~ICameraSettings() = default;
    /// <summary>
    /// 同じ内容のコピーを作ります。スナップショットを取るときに使います。
    /// </summary>
    virtual std::unique_ptr<ICameraSettings> Clone() const = 0;
};

/// <summary>
/// 全パイプライン共通のカメラ設定です。
/// </summary>
struct CameraCommon
{
    Matrix view;
    Matrix projection;
    // 前フレームのView * Projection。Updateで自動的に入る（モーションベクトルなど用）
    Matrix previous_view_projection;
    Vector3 position;
    Viewport viewport;
    // 描画先。バックバッファかRenderTexture
    TextureHandle color_target;
    TextureHandle depth_target;
    // MeshRendererData::layer_maskとANDを取り、0なら描かない
    uint32_t culling_mask = UINT32_MAX;
    // 小さい順に描く
    int32_t priority = 0;
    LoadOp color_load = LoadOp::Clear;
    LoadOp depth_load = LoadOp::Clear;
    std::array<float, 4> clear_color = {0, 0, 0, 1};
    // デバッグ表示の線とUIを描くかどうか（画面に出すカメラだけtrueにする）
    bool draw_overlays = false;
};

/// <summary>
/// 1フレーム分のカメラのコピーです。描画中にゲーム側が値を変えても影響を受けません。
/// </summary>
struct CameraSnapshot
{
    CameraCommon common;
    std::unique_ptr<ICameraSettings> settings;
};

/// <summary>
/// カメラのデータを持つストアです。ゲーム側が毎フレームUpdateし、描画の前にスナップショットを取ります。
/// </summary>
class CameraDataStore
{
    struct Entry
    {
        CameraCommon common;
        std::unique_ptr<ICameraSettings> settings;
        // 一度でもUpdateされたか。されていないカメラは描かない
        bool has_previous = false;
    };

    SlotPool<CameraTag, Entry> m_cameras_;

public:
    /// <summary>
    /// カメラを登録します。最初のUpdateまでは描きません。
    /// </summary>
    /// <param name="settings">パイプライン固有の設定（nullptrでもよい）</param>
    CameraId Create(std::unique_ptr<ICameraSettings> settings);
    /// <summary>
    /// カメラを取り除きます。
    /// </summary>
    void Destroy(CameraId id);

    /// <summary>
    /// 共通設定を更新します。前フレームのViewProjは自動で残します。
    /// </summary>
    void Update(CameraId id, const CameraCommon &common);

    /// <summary>
    /// 共通設定を取得します。無効なIDならnullptrを返します。
    /// </summary>
    const CameraCommon *GetCommon(CameraId id) const;
    /// <summary>
    /// パイプライン固有の設定を取得します。パイプラインの型にキャストして使います。
    /// </summary>
    ICameraSettings *GetSettings(CameraId id);

    /// <summary>
    /// Updateされたことのあるカメラのコピーを作ります。
    /// </summary>
    std::vector<CameraSnapshot> TakeSnapshot() const;
};
}
