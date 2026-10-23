#pragma once
#include <memory>
#include <vector>

#include "Rendering/CaboRenderingSystem/Core/math_types.h"
#include "Rendering/CaboRenderingSystem/Core/slot_pool.h"
#include "Rendering/CaboRenderingSystem/Material/material_id.h"
#include "Rendering/CaboRenderingSystem/RHI/rhi_types.h"

namespace CRS
{
using MeshRendererId = Handle<struct MeshRendererTag>;

/// <summary>
/// インデックスバッファの一部を1つのマテリアルで描く範囲です。サブメッシュごとに1つ持ちます。
/// </summary>
struct DrawRange
{
    uint32_t first_index = 0;
    uint32_t index_count = 0;
    int32_t base_vertex = 0;
    MaterialId material;
};

/// <summary>
/// メッシュの描画に必要な最小限のデータです。ゲーム側のオブジェクトへの参照は持ちません。
/// </summary>
struct MeshRendererData
{
    BufferHandle vertex_buffer;
    BufferHandle index_buffer;
    std::shared_ptr<const VertexLayout> vertex_layout;
    // スキニング用のボーン行列（StructuredBuffer<float4x4>）。無効ならスキニングしない
    BufferHandle bone_buffer;
    std::vector<DrawRange> ranges;
    Matrix world;
    Matrix previous_world;
    BoundingBox local_bounds;
    uint32_t layer_mask = 1;
    bool cast_shadow = true;
};

/// <summary>
/// メッシュの描画データを持つストアです。ゲーム側が登録・更新し、エンジンがスナップショットを取ります。
/// </summary>
class MeshRendererDataStore
{
    SlotPool<MeshRendererTag, MeshRendererData> m_data_;

public:
    MeshRendererId Register(MeshRendererData data);
    void Unregister(MeshRendererId id);
    bool IsValid(MeshRendererId id) const;

    /// <summary>
    /// データを差し替えます。ワールド行列は、前フレームの値をprevious_worldに残してから更新します。
    /// </summary>
    void Update(MeshRendererId id, MeshRendererData data);
    void UpdateTransform(MeshRendererId id, const Matrix &world);

    const MeshRendererData *Get(MeshRendererId id) const;
    size_t Count() const;

    /// <summary>
    /// 読み取り専用のコピーを作ります。
    /// </summary>
    std::vector<MeshRendererData> TakeSnapshot() const;
};
}
