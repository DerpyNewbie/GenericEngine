#include "pch.h"

#include "VertexBuffer.h"

#include "DirectXResourceFactory.h"
#include "RenderEngine.h"
#include "Rendering/Vertex.h"
#include "Components/skinned_mesh_renderer.h"

namespace engine
{

std::vector<Vertex> VertexBuffer::CreateVertexData(const Mesh *mesh) const
{
    std::vector<Vertex> vertices;
    vertices.resize(mesh->vertices.size());
    for (size_t i = 0; i < mesh->vertices.size(); ++i)
    {
        vertices.at(i).vertex = mesh->vertices[i];
    }
    for (size_t i = 0; i < mesh->colors.size(); ++i)
    {
        vertices.at(i).color = mesh->colors[i];
    }
    for (size_t i = 0; i < mesh->normals.size(); ++i)
    {
        vertices.at(i).normal = mesh->normals[i];
    }
    for (size_t i = 0; i < mesh->tangents.size(); ++i)
    {
        vertices.at(i).tangent = mesh->tangents[i];
    }
    for (size_t i = 0; i < mesh->uvs.size(); ++i)
    {
        for (size_t j = 0; j < mesh->uvs[i].size(); ++j)
        {
            vertices.at(j).uvs[i] = mesh->uvs[i][j];
        }
    }
    for (size_t i = 0; i < mesh->bone_weights.size(); ++i)
    {
        vertices.at(i).bones_per_vertex = static_cast<unsigned int>(mesh->bone_weights[i].size());
        for (size_t j = 0; j < mesh->bone_weights[i].size(); ++j)
        {
            vertices.at(i).bone_index[j] = mesh->bone_weights[i][j].bone_index;
            vertices.at(i).bone_weight[j] = mesh->bone_weights[i][j].weight;
        }
    }

    return vertices;
}

VertexBuffer::VertexBuffer(const Mesh *init_data) : VertexBuffer(init_data->vertices.size(), CreateVertexData(init_data).data())
{}

VertexBuffer::VertexBuffer(const size_t num_vertices, const Vertex *init_data)
{
    auto size = sizeof(Vertex) * num_vertices;
    constexpr auto stride = sizeof(Vertex);

    const auto upload_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    const auto desc = CD3DX12_RESOURCE_DESC::Buffer(size);

    m_buffer_ = DirectXResourceFactory::CreateUploadedBuffer(
        init_data,
        size);

    m_buffer_->SetName(L"VertexBuffer");

    m_view_.BufferLocation = m_buffer_->GetGPUVirtualAddress();
    m_view_.SizeInBytes = static_cast<UINT>(size);
    m_view_.StrideInBytes = static_cast<UINT>(stride);
}

D3D12_VERTEX_BUFFER_VIEW *VertexBuffer::View()
{
    return &m_view_;
}

bool VertexBuffer::IsValid() const
{
    return m_buffer_ != nullptr;
}
}