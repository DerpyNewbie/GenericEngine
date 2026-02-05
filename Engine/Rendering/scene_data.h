#pragma once

namespace engine
{
struct SceneData
{
    Vector2 screen_size;
    Vector2 shadow_map_size;
    Vector3 camera_pos;
    float time;
    float delta_time;
};
}