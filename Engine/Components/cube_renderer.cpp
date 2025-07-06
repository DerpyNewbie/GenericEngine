#include "pch.h"

#include "cube_renderer.h"

#include "game_object.h"
#include "transform.h"
#include "DxLib/dxlib_converter.h"

namespace engine
{
void CubeRenderer::OnDraw()
{
    const auto matrix = DxLibConverter::From(GameObject()->Transform()->WorldToLocal());
    const auto half_extent = VScale(DxLibConverter::From(extent), 0.5F);
    // DrawBox(0, 0, 500, 500, diffuse_color.Argb(), true);
    DrawCube3D(VTransform(half_extent, matrix), VTransform(VScale(half_extent, -1), matrix),
               DxLibConverter::Argb(diffuse_color), DxLibConverter::Argb(specular_color), fill);
}
}

CEREAL_REGISTER_TYPE(engine::CubeRenderer)