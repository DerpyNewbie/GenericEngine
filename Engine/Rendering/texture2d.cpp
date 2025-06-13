#include "texture2d.h"

#include "Math/color.h"

#include <DxLib.h>

namespace engine
{
Texture2D Texture2D::GetColoredTexture(const Color color)
{
    const int handle = MakeSoftImage(2, 2);
    FillSoftImage(handle, color.r, color.g, color.b, color.a);
    
    return Texture2D{handle};
}
}