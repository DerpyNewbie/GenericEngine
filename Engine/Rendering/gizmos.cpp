#include "pch.h"
#include "gizmos.h"

#include "update_manager.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
std::shared_ptr<Gizmos> Gizmos::m_instance_ = nullptr;
std::vector<Vertex> Gizmos::m_vertices_;

void Gizmos::Init()
{
    assert(g_RenderEngine != nullptr && "RenderEngine is not initialized");
    assert(m_instance_ == nullptr && "Gizmos is already initialized");

    m_instance_ = std::make_shared<Gizmos>();
}

void Gizmos::CreateVertexBuffer(const int current_back_buffer_idx)
{
    if (m_vertices_.empty())
    {
        m_vertex_buffers_[current_back_buffer_idx] = nullptr;
        m_vertices_count_[current_back_buffer_idx] = 0;
        return;
    }

    const auto vertex_buffer = std::make_shared<VertexBuffer>(m_vertices_.size(), m_vertices_.data());

    if (!vertex_buffer->IsValid())
    {
        Logger::Error<Gizmos>("Failed to create VertexBuffer: Invalid VertexBuffer");
        m_vertex_buffers_[current_back_buffer_idx] = nullptr;
        m_vertices_count_[current_back_buffer_idx] = 0;
        m_vertices_.clear();
        return;
    }

    // store it, then clear current vertices
    m_instance_->m_vertex_buffers_[current_back_buffer_idx] = vertex_buffer;
    m_instance_->m_vertices_count_[current_back_buffer_idx] = m_vertices_.size();
    m_vertices_.clear();
}

void Gizmos::Render()
{
    auto current_back_buffer_idx = g_RenderEngine->CurrentBackBufferIndex();

    assert(m_instance_ != nullptr && "Gizmos is not initialized");

    // have we created a vertex buffer for this frame? 
    if (current_back_buffer_idx != m_instance_->m_last_back_buffer_idx_)
    {
        // if not, create a new vertex buffer for this frame
        m_instance_->CreateVertexBuffer(current_back_buffer_idx);
        m_instance_->m_last_back_buffer_idx_ = current_back_buffer_idx;
    }

    const auto current_vertex_buffer = m_instance_->m_vertex_buffers_[current_back_buffer_idx];
    const auto current_vertices_count = m_instance_->m_vertices_count_[current_back_buffer_idx];

    // if we had nothing to render, ignore it
    if (current_vertices_count == 0 || current_vertex_buffer == nullptr)
    {
        return;
    }

    // render stuff
    auto command_list = g_RenderEngine->CommandList();
    command_list->SetPipelineState(PSOManager::Get("Line"));
    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    command_list->IASetVertexBuffers(0, 1, current_vertex_buffer->View());
    command_list->DrawInstanced(current_vertices_count, 1, 0, 0);
}

void Gizmos::DrawLine(const Vector3 &start, const Vector3 &end, const Color &color)
{
    Vertex vert_start{.vertex = start, .color = color};
    Vertex vert_end{.vertex = end, .color = color};

    m_vertices_.emplace_back(vert_start);
    m_vertices_.emplace_back(vert_end);
}

void Gizmos::DrawLines(const std::vector<Vector3> &line, const Color &color)
{
    for (size_t i = 0; i < line.size() - 1; i++)
    {
        DrawLine(line[i], line[i + 1], color);
    }
}

void Gizmos::DrawCircle(const Vector3 &center, const float radius, const Color &color,
                        const Quaternion &rotation, const int segments)
{
    std::vector<Vector3> circle(segments);
    for (auto i = 0; i < segments; i++)
    {
        const auto theta = Mathf::kPi * 2 * static_cast<float>(i) / static_cast<float>(segments);
        circle[i] = Vector3{std::cos(theta) * radius, std::sin(theta) * radius, 0};
    }
    for (auto &point : circle)
    {
        Vector3::Transform(point, rotation, point);
        point += center;
    }

    circle.emplace_back(circle[0]);
    DrawLines(circle, color);
}

void Gizmos::DrawSphere(const Vector3 &center, const float radius, const Color &color, const int segments)
{
    for (int i = 0; i < segments; i++)
    {
        const auto progress = static_cast<float>(i) / static_cast<float>(segments);
        const auto theta = Mathf::kPi * 2 * progress;
        DrawCircle(center, radius, color, Quaternion::CreateFromYawPitchRoll(theta, 0, 0), segments);

        // make it uv sphere
        DrawCircle(center, radius, color, Quaternion::CreateFromYawPitchRoll(0, theta, 0), segments);
    }
}

void Gizmos::DrawBounds(const DirectX::BoundingBox &bounds, const Color &color, const Matrix &mat)
{
    Vector3 local[8] = {
        bounds.Center + Vector3(-bounds.Extents.x, -bounds.Extents.y, -bounds.Extents.z),
        bounds.Center + Vector3(bounds.Extents.x, -bounds.Extents.y, -bounds.Extents.z),
        bounds.Center + Vector3(bounds.Extents.x, -bounds.Extents.y, bounds.Extents.z),
        bounds.Center + Vector3(-bounds.Extents.x, -bounds.Extents.y, bounds.Extents.z),
        bounds.Center + Vector3(-bounds.Extents.x, bounds.Extents.y, -bounds.Extents.z),
        bounds.Center + Vector3(bounds.Extents.x, bounds.Extents.y, -bounds.Extents.z),
        bounds.Center + Vector3(bounds.Extents.x, bounds.Extents.y, bounds.Extents.z),
        bounds.Center + Vector3(-bounds.Extents.x, bounds.Extents.y, bounds.Extents.z)
    };

    for (auto &v : local)
        v = Vector3::Transform(v, mat);

    const int e[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    for (auto &p : e)
        DrawLine(local[p[0]], local[p[1]], color);
}
}