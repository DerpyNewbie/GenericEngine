#pragma once
#include <array>
#include <vector>

#include "Rendering/CaboRenderingSystem/Core/math_types.h"
#include "Rendering/CaboRenderingSystem/Material/material_id.h"
#include "Rendering/CaboRenderingSystem/RHI/handles.h"

namespace CRS
{
/// <summary>
/// デバッグ表示の線の頂点です。HLSL側のLineVertexと同じ並びです。
/// </summary>
struct LineVertex
{
    Vector3 position;
    Color color;
};

/// <summary>
/// UIの頂点です。位置は画面の正規化座標（-1..1）です。HLSL側のUiVertexと同じ並びです。
/// </summary>
struct UiVertex
{
    Vector2 position;
    Vector2 uv;
    Color color;
};

/// <summary>
/// 同じテクスチャで続けて描くUIの頂点の範囲です。
/// </summary>
struct UiBatch
{
    TextureHandle texture;
    // UI用のShaderのMaterial。無効なら標準のShader(テクスチャ x 頂点色)で描く
    MaterialId material;
    uint32_t first_vertex = 0;
    uint32_t vertex_count = 0;
};

/// <summary>
/// 毎フレーム作り直す描画（デバッグ表示の線、UI）を集めます。フレームの終わりに空にします。
/// </summary>
class ImmediateDrawStore
{
    std::vector<LineVertex> m_lines_;
    std::vector<UiVertex> m_ui_vertices_;
    std::vector<UiBatch> m_ui_batches_;

public:
    void AddLine(const Vector3 &start, const Vector3 &end, const Color &color);
    void AddLines(const LineVertex *vertices, size_t count);

    /// <summary>
    /// 四角形を追加します。cornersは左上、右上、左下、右下の順です。登録順に描きます。
    /// </summary>
    void AddUiQuad(TextureHandle texture, const std::array<Vector2, 4> &corners, const std::array<Vector2, 4> &uvs,
                   const Color &color, MaterialId material);
    void AddUiQuad(TextureHandle texture, const std::array<Vector2, 4> &corners, const std::array<Vector2, 4> &uvs,
                   const Color &color);

    const std::vector<LineVertex> &Lines() const
    {
        return m_lines_;
    }

    const std::vector<UiVertex> &UiVertices() const
    {
        return m_ui_vertices_;
    }

    const std::vector<UiBatch> &UiBatches() const
    {
        return m_ui_batches_;
    }

    void Clear();
};
}
