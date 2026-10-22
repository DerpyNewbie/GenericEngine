#pragma once
#include "Rendering/CaboRenderingSystem/RHI/handles.h"

namespace CRS
{
using MaterialId = Handle<struct MaterialTag>;

/// <summary>
/// パスの種類です。増やすときはここに追加します。
/// </summary>
enum class PassTag : uint8_t
{
    Shadow,
    DepthPrepass,
    Forward,

    Count
};
}