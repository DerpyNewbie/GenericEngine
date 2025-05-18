#include "cube_renderer.h"

#include "DxLib.h"
#include "game_object.h"
#include "transform.h"

namespace engine
{
void CubeRenderer::OnDraw()
{
    const auto matrix = GameObject()->Transform()->LocalToWorld();
    const auto half_extent = VScale(extent, 0.5F);
    // DrawBox(0, 0, 500, 500, diffuse_color.Argb(), true);
    DrawCube3D(VTransform(half_extent, matrix), VTransform(VScale(half_extent, -1), matrix),
               diffuse_color.Argb(), specular_color.Argb(), fill);
}
}

CEREAL_REGISTER_TYPE(engine::CubeRenderer)