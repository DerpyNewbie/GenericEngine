#pragma once
#include <unordered_map>
#include <vector>

#include "Rendering/CaboRenderingSystem/Material/material.h"
#include "Rendering/CaboRenderingSystem/Pipeline/render_view.h"
#include "Rendering/CaboRenderingSystem/RHI/i_render_device.h"
#include "Rendering/CaboRenderingSystem/Scene/frame_snapshot.h"

namespace CRS
{
/// <summary>
/// 描画リストの1要素です。メッシュの1範囲を1つのマテリアルとPSOで描きます。
/// </summary>
struct DrawItem
{
    PipelineStateHandle pipeline_state;
    // スナップショットの中を指す（そのフレームの間だけ有効）
    const MeshRendererData *mesh = nullptr;
    const DrawRange *range = nullptr;
    const Material *material = nullptr;
    // 描く順番。上位16bitがrender_queue。不透明はPSOと手前からの距離、半透明は奥からの距離で並ぶ
    uint64_t sort_key = 0;
};

// sort_keyの小さい順に並んだ描画リスト
using DrawList = std::vector<DrawItem>;

/// <summary>
/// ビューから見えるメッシュを絞り込みます。
/// </summary>
class CullingStage
{
public:
    /// <summary>
    /// レイヤーマスクと視錐台で判定し、見えるメッシュのスナップショット内の番号を返します。
    /// </summary>
    std::vector<uint32_t> Cull(const RenderView &view, const std::vector<MeshRendererData> &meshes) const;
};

/// <summary>
/// パスタグでマテリアルを引き、描画リストを作ります。そのパスのエントリが無いマテリアルは除外します。
/// PSOはマテリアルのパスと描画先の設定から、描画時に解決します。
/// </summary>
class DrawListBuilder
{
    // PSOを決める要素の組み合わせ。デバイス側のキャッシュより手前で引き、PipelineStateDescを作る手間を省く
    struct PipelineKey
    {
        MaterialId material;
        uint32_t material_version = 0;
        PassTag pass = PassTag::Forward;
        const VertexLayout *vertex_layout = nullptr;
        Format color_format = Format::Unknown;
        Format depth_format = Format::Unknown;
        uint32_t sample_count = 1;

        bool operator==(const PipelineKey &) const = default;
    };

    struct PipelineKeyHash
    {
        size_t operator()(const PipelineKey &key) const;
    };

    IRenderDevice &m_device_;
    std::unordered_map<PipelineKey, PipelineStateHandle, PipelineKeyHash> m_pipeline_cache_;

    // マテリアルのパスと、ビューの描画先・メッシュの頂点の並びから、PSOをキャッシュから引く（なければ作る）
    PipelineStateHandle ResolvePipelineState(MaterialId material_id, const Material &material, const MaterialPass &pass,
                                             PassTag tag, const RenderView &view, const MeshRendererData &mesh);

public:
    explicit DrawListBuilder(IRenderDevice &device);

    /// <summary>
    /// 見えるメッシュのサブメッシュごとに、tagのパスを持つマテリアルの描画項目を作り、sort_keyで並べます。
    /// </summary>
    /// <param name="visible">CullingStage::Cullの結果（frame.meshesの番号）</param>
    DrawList Build(PassTag tag, const RenderView &view, const std::vector<uint32_t> &visible,
                   const FrameSnapshot &frame, const MaterialRegistry &materials);
};
}