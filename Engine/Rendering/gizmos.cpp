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

    UpdateManager::SubscribeDrawCall(m_instance_);
}

void Gizmos::OnDraw()
{
    assert(m_instance_ != nullptr && "Gizmos is not initialized");

    if (m_vertices_.empty())
        return;

    const auto current_buffer_idx = g_RenderEngine->CurrentBackBufferIndex();
    const auto vertex_buffer = std::make_shared<VertexBuffer>(m_vertices_.size(), m_vertices_.data());

    if (!vertex_buffer->IsValid())
    {
        Logger::Error<Gizmos>("Failed to create VertexBuffer!");
        m_vertices_.clear();
        return;
    }

    m_instance_->m_vertex_buffers_[current_buffer_idx] = vertex_buffer;

    const auto cmd_list = g_RenderEngine->CommandList();

    cmd_list->SetPipelineState(PSOManager::Get("Line"));
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    cmd_list->IASetVertexBuffers(0, 1, vertex_buffer->View());
    cmd_list->DrawInstanced(m_vertices_.size(), 1, 0, 0);

    m_vertices_.clear();
}

void Gizmos::DrawLine(const Vector3 &start, const Vector3 &end, const Color &color)
{
    Vertex vert_start{start, color};
    Vertex vert_end{end, color};

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