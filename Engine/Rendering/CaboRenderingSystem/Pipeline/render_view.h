#pragma once
#include <array>

#include "Rendering/CaboRenderingSystem/Core/math_types.h"
#include "Rendering/CaboRenderingSystem/RHI/rhi_types.h"
#include "Rendering/CaboRenderingSystem/Scene/camera_data_store.h"

namespace CRS
{
enum class RenderViewKind : uint8_t
{
    Camera,
    Shadow,
};

/// <summary>
/// 「どこから、どこへ、何を描くか」です。カメラ由来も影由来も同じ形にし、同じ経路で処理します。
/// </summary>
struct RenderView
{
    RenderViewKind kind = RenderViewKind::Camera;

    Matrix view;
    Matrix projection;
    Matrix view_projection;
    Matrix previous_view_projection;
    Vector3 position;

    Viewport viewport;
    ScissorRect scissor;

    TextureHandle color_target;
    TextureHandle depth_target;
    // 深度の描画先が配列テクスチャの場合のスライス
    uint32_t depth_slice = kAllSlices;
    Format color_format = Format::Unknown;
    Format depth_format = Format::Unknown;
    uint32_t sample_count = 1;
    LoadOp color_load = LoadOp::Load;
    LoadOp depth_load = LoadOp::Load;
    std::array<float, 4> clear_color = {0, 0, 0, 1};

    uint32_t culling_mask = UINT32_MAX;
    // 視錐台カリングをするかどうか（影のビューは、ビューの外から影を落とすものがあるので行わない）
    bool frustum_cull = true;
    // 深度だけを描くビュー（影など）
    bool depth_only = false;
    // デバッグ表示の線とUIを描くかどうか
    bool draw_overlays = false;

    const ICameraSettings *camera_settings = nullptr;
};

/// <summary>
/// シェーダーに渡すライティングの定数です。HLSL側のcbuffer Lightingと同じ並びです。
/// </summary>
struct LightingConstants
{
    uint32_t light_count = 0;
    uint32_t shadow_map_count = 0;
    Vector2 shadow_map_size;
    Vector4 cascade_splits;
    Color ambient;
};

/// <summary>
/// 1つのカメラビューを描くときのライティングのリソースです。
/// </summary>
struct FrameLighting
{
    LightingConstants constants;
    BufferHandle lights;
    BufferHandle light_view_projections;
    TextureHandle shadow_maps;
};

/// <summary>
/// シェーダーに渡すビュー定数です。HLSL側のcbuffer Viewと同じ並びです。
/// </summary>
struct ViewConstants
{
    Matrix view;
    Matrix projection;
    Matrix view_projection;
    Matrix previous_view_projection;
    Vector4 camera_position;
    // x: 起動からの時間, y: 前フレームからの時間
    Vector4 time;
};

/// <summary>
/// シェーダーに渡すオブジェクト定数です。HLSL側のcbuffer Objectと同じ並びです。
/// </summary>
struct ObjectConstants
{
    static constexpr uint32_t kFlagSkinned = 1;

    Matrix world;
    Matrix previous_world;
    uint32_t flags = 0;
    uint32_t padding[3] = {};
};
}