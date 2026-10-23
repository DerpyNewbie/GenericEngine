#include "Rendering/CaboRenderingSystem/Scene/immediate_draw_store.h"

namespace CRS
{
void ImmediateDrawStore::AddLine(const Vector3 &start, const Vector3 &end, const Color &color)
{
    m_lines_.push_back({start, color});
    m_lines_.push_back({end, color});
}

void ImmediateDrawStore::AddLines(const LineVertex *vertices, const size_t count)
{
    m_lines_.insert(m_lines_.end(), vertices, vertices + count);
}

void ImmediateDrawStore::AddUiQuad(const TextureHandle texture, const std::array<Vector2, 4> &corners,
                                   const std::array<Vector2, 4> &uvs, const Color &color)
{
    AddUiQuad(texture, corners, uvs, color, {});
}

void ImmediateDrawStore::AddUiQuad(const TextureHandle texture, const std::array<Vector2, 4> &corners,
                                   const std::array<Vector2, 4> &uvs, const Color &color, const MaterialId material)
{
    // 2つの三角形（左上, 右上, 左下）（左下, 右上, 右下）
    constexpr int kOrder[6] = {0, 1, 2, 2, 1, 3};

    // 同じテクスチャとMaterialが続く間は1回で描く
    if (m_ui_batches_.empty() || m_ui_batches_.back().texture != texture || m_ui_batches_.back().material != material)
        m_ui_batches_.push_back({texture, material, static_cast<uint32_t>(m_ui_vertices_.size()), 0});

    for (const auto index : kOrder)
        m_ui_vertices_.push_back({corners[index], uvs[index], color});

    m_ui_batches_.back().vertex_count += 6;
}

void ImmediateDrawStore::Clear()
{
    m_lines_.clear();
    m_ui_vertices_.clear();
    m_ui_batches_.clear();
}
}
