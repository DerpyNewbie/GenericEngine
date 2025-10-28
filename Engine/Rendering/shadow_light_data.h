#pragma once

namespace engine
{
struct ShadowLightData
{
    float near_plane;
    float far_plane;
    Matrix view_proj;
};
}