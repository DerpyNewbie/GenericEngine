#pragma once
#include "Rendering/vertex.h"

namespace engine
{
class Mesh;
/// <summary>
/// Meshの頂点を持つBufferです。
/// </summary>
class VertexBuffer
{
    ComPtr<ID3D12Resource> m_buffer_ = nullptr;
    D3D12_VERTEX_BUFFER_VIEW m_view_ = {};

    /// <summary>
    /// Meshの頂点、色、法線、接線、UV、Boneの情報を、Vertexの配列にまとめます。
    /// </summary>
    std::vector<Vertex> CreateVertexData(const Mesh *mesh) const;
    
public:
    /// <summary>
    /// Meshの頂点のデータを書き込んだBufferを作成します。
    /// </summary>
    /// <param name="init_data">元になるMesh</param>
    VertexBuffer(const Mesh *init_data);
    /// <summary>
    /// 頂点のデータを書き込んだBufferを作成します。
    /// </summary>
    /// <param name="num_vertices">頂点の数</param>
    /// <param name="init_data">頂点のデータ</param>
    VertexBuffer(size_t num_vertices, const Vertex *init_data);

    /// <summary>
    /// VertexBufferViewを取得します。
    /// </summary>
    D3D12_VERTEX_BUFFER_VIEW *View();
    /// <summary>
    /// Bufferが作成済みであるかどうかを取得します。
    /// </summary>
    bool IsValid() const;

    VertexBuffer(const VertexBuffer &) = delete;
    void operator =(const VertexBuffer &) = delete;
};
}