#pragma once

namespace engine
{
/// <summary>
/// Shaderに渡すScene全体の情報(画面サイズ、時間など)です。
/// </summary>
struct SceneData
{
    Vector2 screen_size;
    Vector2 shadow_map_size;
    float time;
    float delta_time;
};
}