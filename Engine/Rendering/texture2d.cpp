#include "texture2d.h"

#include "Math/color.h"

#include <DxLib.h>

namespace engine
{
Texture2D Texture2D::GetColoredTexture(const Color color)
{
    const int soft_handle = MakeSoftImage(2, 2);
    FillSoftImage(soft_handle, color.r, color.g, color.b, color.a);
    const int graphic_handle = CreateGraphFromSoftImage(soft_handle);
    DeleteSoftImage(soft_handle);

    return Texture2D{graphic_handle};
}
}