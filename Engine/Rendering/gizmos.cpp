#include "pch.h"
#include "gizmos.h"

#include "update_manager.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "Components/camera.h"
#include "Math/mathf.h"

namespace engine
{
std::shared_ptr<Gizmos> Gizmos::m_instance_;
std::vector<Vertex> Gizmos::m_vertices_;

void Gizmos::Init()
{
    assert(g_RenderEngine != nullptr && "Gizmos is already initialized");

    m_instance_ = std::make_shared<Gizmos>();
    UpdateManager::SubscribeDrawCall(m_instance_);
}
void Gizmos::OnDraw()
{
    if (m_vertices_.empty())
        return;

    const auto camera = Camera::Main();
    auto constant_buffer = ConstantBuffer(sizeof(Matrix) * 2);
    const auto mat_ptr = constant_buffer.GetPtr<Matrix>();
    mat_ptr[0] = camera.lock()->GetViewMatrix();
    mat_ptr[1] = camera.lock()->GetProjectionMatrix();

    const auto vertex_buffer = VertexBuffer(m_vertices_.size(), m_vertices_.data());
    const auto vertex_buffer_view = vertex_buffer.View();
    auto cmd_list = g_RenderEngine->CommandList();

    cmd_list->SetPipelineState(g_PSOManager.Get("Line"));
    cmd_list->SetGraphicsRootConstantBufferView(0, constant_buffer.GetAddress());
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
    cmd_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
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
        const auto theta = Mathf::kPi * 2 * static_cast<float>(i / segments);
        circle.emplace_back(std::cos(theta) * radius, std::sin(theta) * radius, 0);
    }
    for (auto &point : circle)
    {
        Vector3::Transform(point, rotation, point);
        point += center;
    }

    circle.emplace_back(circle[0]);
    DrawLines(circle, color);
}
}