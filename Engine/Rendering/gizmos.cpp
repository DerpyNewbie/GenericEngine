#include "pch.h"
#include "gizmos.h"

#include "update_manager.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/RootSignature.h"
#include "Components/camera.h"

namespace engine
{
std::shared_ptr<Gizmos> Gizmos::m_instance_ = nullptr;
std::vector<Vertex> Gizmos::m_vertices_;

void Gizmos::Init()
{
    assert(g_RenderEngine != nullptr && "RenderEngine is not initialized");
    assert(m_instance_ == nullptr && "Gizmos is already initialized");

    m_instance_ = std::make_shared<Gizmos>();
    m_instance_->m_constant_buffer_ = std::make_shared<ConstantBuffer>(sizeof(Matrix) * 2);
    m_instance_->m_constant_buffer_->CreateBuffer();
    m_instance_->m_desc_handle_ = m_instance_->m_constant_buffer_->UploadBuffer();

    UpdateManager::SubscribeDrawCall(m_instance_);
}

void Gizmos::OnDraw()
{
    assert(m_instance_ != nullptr && "Gizmos is not initialized");

    if (m_vertices_.empty())
        return;

    const auto camera = Camera::Main();
    if (camera == nullptr)
    {
        Logger::Error<Gizmos>("Main Camera is not set!");
        m_vertices_.clear();
        return;
    }

    const auto constant_buffer = m_instance_->m_constant_buffer_;

    if (!constant_buffer->IsValid())
    {
        Logger::Error<Gizmos>("ConstantBuffer is invalid!");
        m_vertices_.clear();
        return;
    }

    Matrix mat[2];
    mat[0] = camera->GetViewMatrix();
    mat[1] = camera->GetProjectionMatrix();
    constant_buffer->UpdateBuffer(mat);

    const auto buffer_index = g_RenderEngine->CurrentBackBufferIndex();
    m_instance_->m_vertex_buffers_[buffer_index] = std::make_shared<VertexBuffer>(
        m_vertices_.size(), m_vertices_.data());
    const auto &vertex_buffer = m_instance_->m_vertex_buffers_[buffer_index];
    if (!vertex_buffer->IsValid())
    {
        Logger::Error<Gizmos>("Failed to create VertexBuffer!");
        m_vertices_.clear();
        return;
    }

    const auto vertex_buffer_view = vertex_buffer->View();
    const auto cmd_list = g_RenderEngine->CommandList();

    cmd_list->SetPipelineState(PSOManager::Get("Line"));
    cmd_list->SetGraphicsRootDescriptorTable(kVertexCBV, m_instance_->m_desc_handle_->HandleGPU);
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
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