#pragma once
#include <vector>

#include "Rendering/CaboRenderingSystem/Scene/camera_data_store.h"
#include "Rendering/CaboRenderingSystem/Scene/immediate_draw_store.h"
#include "Rendering/CaboRenderingSystem/Scene/light_data_store.h"
#include "Rendering/CaboRenderingSystem/Scene/mesh_renderer_data_store.h"
#include "Rendering/CaboRenderingSystem/Scene/scene_environment.h"

namespace CRS
{
/// <summary>
/// 1フレーム分の読み取り専用のコピーです。パイプラインはこれだけを読みます。
/// </summary>
struct FrameSnapshot
{
    std::vector<MeshRendererData> meshes;
    std::vector<CameraSnapshot> cameras;
    std::vector<LightData> lights;
    SceneEnvironment environment;
    // 線とUIは毎フレーム作り直すので、コピーせずに参照する（描画中は変更されない）
    const ImmediateDrawStore *immediate = nullptr;
    // 起動からの秒数と、前フレームからの秒数（シェーダーのView.timeに入る）
    float time = 0.0f;
    float delta_time = 0.0f;
};
}