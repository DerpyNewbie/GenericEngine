#pragma once
#include <array>

#include "Rendering/CaboRenderingSystem/Core/math_types.h"
#include "Rendering/CaboRenderingSystem/RHI/handles.h"

namespace CRS
{
constexpr uint32_t kShadowCascadeCount = 3;

/// <summary>
/// シーン全体で1つの描画設定です（スカイボックスや影の分割など）。
/// </summary>
struct SceneEnvironment
{
    // TextureCube。無効ならスカイボックスを描かない
    TextureHandle skybox;
    // カメラからの距離で、影のカスケードを切り替える位置
    std::array<float, kShadowCascadeCount> cascade_splits = {10.0f, 30.0f, 100.0f};
    // ライトが1つも無い場合の明るさ
    Color ambient = Color(0.25f, 0.25f, 0.25f, 1.0f);
};
}
