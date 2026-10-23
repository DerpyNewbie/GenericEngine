#pragma once
#include <array>
#include <memory>
#include <vector>

#include "Rendering/CaboRenderingSystem/Core/math_types.h"
#include "Rendering/CaboRenderingSystem/Core/slot_pool.h"
#include "Rendering/CaboRenderingSystem/RHI/rhi_types.h"

namespace CRS
{
using CameraId = Handle<struct CameraTag>;

/// <summary>
/// パイプライン固有のカメラ設定です。パイプラインが生成するので、受け側はキャストだけで使えます。
/// </summary>
class ICameraSettings
{
public:
    virtual ~ICameraSettings() = default;
    virtual std::unique_ptr<ICameraSettings> Clone() const = 0;
};

/// <summary>
/// 全パイプライン共通のカメラ設定です。
/// </summary>
struct CameraCommon
{
    Matrix view;
    Matrix projection;
    Matrix previous_view_projection;
    Vector3 position;
    Viewport viewport;
    TextureHandle color_target;
    TextureHandle depth_target;
    uint32_t culling_mask = UINT32_MAX;
    int32_t priority = 0;
    LoadOp color_load = LoadOp::Clear;
    LoadOp depth_load = LoadOp::Clear;
    std::array<float, 4> clear_color = {0, 0, 0, 1};
    // デバッグ表示の線とUIを描くかどうか（画面に出すカメラだけtrueにする）
    bool draw_overlays = false;
};

struct CameraSnapshot
{
    CameraCommon common;
    std::unique_ptr<ICameraSettings> settings;
};

/// <summary>
/// カメラのデータを持つストアです。
/// </summary>
class CameraDataStore
{
    struct Entry
    {
        CameraCommon common;
        std::unique_ptr<ICameraSettings> settings;
        bool has_previous = false;
    };

    SlotPool<CameraTag, Entry> m_cameras_;

public:
    CameraId Create(std::unique_ptr<ICameraSettings> settings);
    void Destroy(CameraId id);

    /// <summary>
    /// 共通設定を更新します。前フレームのViewProjは自動で残します。
    /// </summary>
    void Update(CameraId id, const CameraCommon &common);

    const CameraCommon *GetCommon(CameraId id) const;
    ICameraSettings *GetSettings(CameraId id);

    std::vector<CameraSnapshot> TakeSnapshot() const;
};
}
