#pragma once
#include <d3d12.h>
#include "ComPtr.h"
#include "../../mesh.h"

namespace engine
{
class VertexBuffer
{
public:
    VertexBuffer(std::shared_ptr<Mesh> pInitData); // コンストラクタでバッファを生成
    D3D12_VERTEX_BUFFER_VIEW View() const; // 頂点バッファビューを取得
    bool IsValid(); // バッファの生成に成功したかを取得

private:
    bool m_IsValid = false; // バッファの生成に成功したかを取得
    ComPtr<ID3D12Resource> m_pBuffer = nullptr; // バッファ
    D3D12_VERTEX_BUFFER_VIEW m_View = {}; // 頂点バッファビュー

    VertexBuffer(const VertexBuffer &) = delete;
    void operator =(const VertexBuffer &) = delete;
};
}