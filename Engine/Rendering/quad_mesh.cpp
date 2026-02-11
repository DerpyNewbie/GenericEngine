#include "pch.h"
#include "quad_mesh.h"

std::shared_ptr<engine::Mesh> QuadMesh::GetMesh()
{
    if (m_mesh_)
        return m_mesh_;

    m_mesh_ = std::make_shared<engine::Mesh>();
    
    m_mesh_->vertices.emplace_back(Vector3(-1, -1, 0.0f));
    m_mesh_->vertices.emplace_back(Vector3(-1, 1, 0.0f));
    m_mesh_->vertices.emplace_back(Vector3(1, -1, 0.0f));
    m_mesh_->vertices.emplace_back(Vector3(1, 1, 0.0f));

    m_mesh_->uvs[0].emplace_back(Vector2(0, 1));
    m_mesh_->uvs[0].emplace_back(Vector2(0, 0));
    m_mesh_->uvs[0].emplace_back(Vector2(1, 1));
    m_mesh_->uvs[0].emplace_back(Vector2(1, 0));

    m_mesh_->indices = {0, 1, 2, 2, 1, 3};

    return m_mesh_;
}