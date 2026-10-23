#pragma once
#include "Rendering/CaboRenderingSystem/RHI/handles.h"

namespace CRS
{
// MaterialRegistryに登録したマテリアルを指すID
using MaterialId = Handle<struct MaterialTag>;

/// <summary>
/// パスの種類です。Materialはパスごとにシェーダーと描画設定を持ちます。増やすときはここに追加します。
/// </summary>
enum class PassTag : uint8_t
{
    Shadow,
    // 予約（まだ使っているパスはありません）
    DepthPrepass,
    Forward,

    Count
};
}