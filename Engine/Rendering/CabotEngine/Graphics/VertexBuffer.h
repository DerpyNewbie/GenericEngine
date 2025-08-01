#pragma once
#include "../../mesh.h"
#include "Rendering/vertex.h"

namespace engine
{
class VertexBuffer
{
    bool m_IsValid = false; // バッファの生成に成功したかを取得
    ComPtr<ID3D12Resource> m_pBuffer = nullptr; // バッファ
    D3D12_VERTEX_BUFFER_VIEW m_View = {}; // 頂点バッファビュー

public:
    VertexBuffer(const Mesh *p_init_data); // コンストラクタでバッファを生成
    VertexBuffer(UINT num_vertices, const Vertex *p_init_data);
    D3D12_VERTEX_BUFFER_VIEW View() const; // 頂点バッファビューを取得
    bool IsValid(); // バッファの生成に成功したかを取得

    VertexBuffer(const VertexBuffer &) = delete;
    void operator =(const VertexBuffer &) = delete;
};
}