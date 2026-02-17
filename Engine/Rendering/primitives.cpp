#include "pch.h"
#include "primitives.h"

std::shared_ptr<engine::Mesh> Primitives::GetQuadMesh()
{
    if (m_quad_mesh_)
        return m_quad_mesh_;

    m_quad_mesh_ = std::make_shared<engine::Mesh>();

    m_quad_mesh_->vertices.emplace_back(Vector3(-1, -1, 0.0f));
    m_quad_mesh_->vertices.emplace_back(Vector3(-1, 1, 0.0f));
    m_quad_mesh_->vertices.emplace_back(Vector3(1, -1, 0.0f));
    m_quad_mesh_->vertices.emplace_back(Vector3(1, 1, 0.0f));

    m_quad_mesh_->uvs[0].emplace_back(Vector2(0, 1));
    m_quad_mesh_->uvs[0].emplace_back(Vector2(0, 0));
    m_quad_mesh_->uvs[0].emplace_back(Vector2(1, 1));
    m_quad_mesh_->uvs[0].emplace_back(Vector2(1, 0));

    m_quad_mesh_->indices = {0, 1, 2, 2, 1, 3};

    return m_quad_mesh_;
}