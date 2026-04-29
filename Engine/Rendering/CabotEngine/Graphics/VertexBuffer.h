#pragma once
#include "Rendering/vertex.h"

namespace engine
{
class Mesh;
class VertexBuffer
{
    ComPtr<ID3D12Resource> m_upload_buffer_ = nullptr;
    ComPtr<ID3D12Resource> m_default_buffer_ = nullptr;
    D3D12_VERTEX_BUFFER_VIEW m_view_ = {};

    std::vector<Vertex> CreateVertexData(const Mesh *mesh) const;
    
public:
    VertexBuffer(const Mesh *init_data);
    VertexBuffer(size_t num_vertices, const Vertex *init_data);

    D3D12_VERTEX_BUFFER_VIEW *View();
    bool IsValid() const;

    VertexBuffer(const VertexBuffer &) = delete;
    void operator =(const VertexBuffer &) = delete;
};
}