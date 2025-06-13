#pragma once
#include "Math/color.h"

namespace engine
{
struct Texture2D
{
    int graphic_handle = -1;

    static Texture2D GetColoredTexture(Color color);
};
}