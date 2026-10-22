#pragma once
#include <vector>

#include "Rendering/CaboRenderingSystem/Pipeline/render_view.h"
#include "Rendering/CaboRenderingSystem/RHI/gpu_resources.h"
#include "Rendering/CaboRenderingSystem/Scene/frame_snapshot.h"

namespace CRS
{
/// <summary>
/// シェーダーに渡すライトです。HLSL側のGpuLightと同じ並びです（StructuredBufferなので詰めて並べる）。
/// </summary>
struct GpuLight
{
    int32_t type = 0;
    int32_t cast_shadow = 0;
    float intensity = 0;
    float range = 0;
    Vector3 position;
    int32_t shadow_index = 0;
    Vector3 direction;
    float inner_cos = 0;
    Color color;
    float outer_cos = 0;
    float padding[3] = {};
};

static_assert(sizeof(GpuLight) == 80);

/// <summary>
/// ライトとカメラから、影のビュー（カスケード、スポットライト）、影のテクスチャ、シェーダー用のライトのデータを作ります。
/// パイプラインからは、影由来のRenderViewとして普通に流れるだけです。
/// </summary>
class ShadowSystem
{
    struct BufferSet
    {
        StructuredBuffer<GpuLight> lights;
        StructuredBuffer<Matrix> light_view_projections;
    };

    IRenderDevice &m_device_;
    uint32_t m_map_size_;
    uint32_t m_max_maps_;
    Texture m_shadow_maps_;
    // 1フレームに複数のカメラを描くので、カメラごとにバッファを使い分ける
    std::vector<BufferSet> m_buffer_sets_;
    uint32_t m_next_buffer_set_ = 0;

public:
    static constexpr uint32_t kMaxLights = 16;

    ShadowSystem(IRenderDevice &device, uint32_t map_size, uint32_t max_maps);

    /// <summary>
    /// フレームの始めに呼びます。
    /// </summary>
    void BeginFrame();

    /// <summary>
    /// カメラのビューに対するライティングのデータを作り、影のビューを追加します。
    /// </summary>
    FrameLighting Prepare(const FrameSnapshot &frame, const RenderView &camera_view,
                          std::vector<RenderView> &out_shadow_views);
};
}