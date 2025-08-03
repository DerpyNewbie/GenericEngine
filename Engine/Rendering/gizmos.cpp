#include "pch.h"
#include "gizmos.h"

#include "update_manager.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/RootSignature.h"
#include "Components/camera.h"
#include "DxLib/dxlib_helper.h"
#include "Editor/editor.h"
#include "Editor/editor_prefs.h"

namespace engine
{
std::shared_ptr<Gizmos> Gizmos::m_instance_ = nullptr;
std::vector<Vertex> Gizmos::m_vertices_;
bool Gizmos::m_has_drawn_;

void Gizmos::Init()
{
    assert(g_RenderEngine != nullptr && "RenderEngine is not initialized");
    assert(m_instance_ == nullptr && "Gizmos is already initialized");

    m_instance_ = std::make_shared<Gizmos>();
    m_instance_->m_constant_buffer_ = std::make_shared<ConstantBuffer>(sizeof(Matrix) * 2);
    m_instance_->m_constant_buffer_->CreateBuffer();
    m_instance_->m_desc_handle_ = m_instance_->m_constant_buffer_->UploadBuffer();
}

void Gizmos::ClearVertices()
{
    m_vertices_.clear();
    m_has_drawn_ = false;
}

void Gizmos::PreDrawCheck()
{
    if (m_has_drawn_)
    {
        ClearVertices();
    }
}

void Gizmos::Render()
{
    assert(m_instance_ != nullptr && "Gizmos is not initialized");

    if (editor::EditorPrefs::show_grid)
    {
        DrawYPlaneGrid();
    }

    auto a = m_vertices_.size();
    if (m_vertices_.empty())
        return;

    const auto camera = Camera::Current().lock();
    if (camera == nullptr)
    {
        Logger::Error<Gizmos>("Main Camera is not set!");
        return;
    }

    const auto constant_buffer = m_instance_->m_constant_buffer_;

    if (!constant_buffer->IsValid())
    {
        Logger::Error<Gizmos>("ConstantBuffer is invalid!");
        return;
    }

    Matrix mat[2];
    mat[0] = camera->GetViewMatrix();
    mat[1] = camera->GetProjectionMatrix();
    constant_buffer->UpdateBuffer(mat);

    auto buffer_index = g_RenderEngine->CurrentBackBufferIndex();

    if (!m_has_drawn_)
        m_instance_->m_vertex_buffers_[buffer_index] = std::make_shared<VertexBuffer>(
            m_vertices_.size(), m_vertices_.data());

    const auto &vertex_buffer = m_instance_->m_vertex_buffers_[buffer_index];
    if (!vertex_buffer->IsValid())
    {
        Logger::Error<Gizmos>("Failed to create VertexBuffer!");
        return;
    }

    const auto vertex_buffer_view = vertex_buffer->View();
    const auto cmd_list = g_RenderEngine->CommandList();

    cmd_list->SetPipelineState(PSOManager::Get("Line"));
    cmd_list->SetGraphicsRootDescriptorTable(kVertexCBV, m_instance_->m_desc_handle_->HandleGPU);
    cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    cmd_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
    cmd_list->DrawInstanced(m_vertices_.size(), 1, 0, 0);

    m_has_drawn_ = true;
}

void Gizmos::DrawLine(const Vector3 &start, const Vector3 &end, const Color &color)
{
    if (m_has_drawn_)
        return;

    Vertex vert_start{start, color};
    Vertex vert_end{end, color};

    auto buffer_idx = g_RenderEngine->CurrentBackBufferIndex();
    m_vertices_.emplace_back(vert_start);
    m_vertices_.emplace_back(vert_end);
    auto a = m_vertices_.size();
}

void Gizmos::DrawLines(const std::vector<Vector3> &line, const Color &color)
{
    if (m_has_drawn_)
        return;

    for (size_t i = 0; i < line.size() - 1; i++)
    {
        DrawLine(line[i], line[i + 1], color);
    }
}

void Gizmos::DrawCircle(const Vector3 &center, const float radius, const Color &color,
                        const Quaternion &rotation, const int segments)
{
    if (m_has_drawn_)
        return;

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
    if (m_has_drawn_)
        return;

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
    if (m_has_drawn_)
        return;

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

void Gizmos::DrawYPlaneGrid(const FLOAT2 spacing, const int count)
{
    const MATRIX camera_view_matrix = MInverse(GetCameraViewMatrix());
    const auto [cam_x, cam_y, cam_z] = VTransform({0, 0, 0}, camera_view_matrix);
    const int half_count = count / 2;
    const int center_x_index = static_cast<int>(cam_x / spacing.u);
    const int center_z_index = static_cast<int>(cam_z / spacing.v);
    const int max_x = center_x_index + half_count;
    const int min_x = center_x_index - half_count;
    const int max_z = center_z_index + half_count;
    const int min_z = center_z_index - half_count;

    const float x_end = spacing.u * static_cast<float>(min_x);
    const float z_end = spacing.v * static_cast<float>(min_z);

    constexpr auto color_default = Color(0.6F, 0.6F, 0.6F, 1.0F);
    constexpr auto color_z = Color(0.6F, 0.6F, 1.0F, 1.0F);
    constexpr auto color_x = Color(1.0F, 0.6F, 0.6F, 1.0F);
    constexpr auto color_y = Color(0.6F, 1.0F, 0.6F, 1.0F);

    for (int z = min_z; z < max_z; ++z)
    {
        for (int x = min_x; x < max_x; ++x)
        {
            const auto dx = spacing.u * static_cast<float>(x);
            const auto dz = spacing.v * static_cast<float>(z);

            const Vector3 z_line_begin = {dx, 0, dz};
            const Vector3 z_line_end = {dx, 0, z_end};

            const Vector3 x_line_begin = {dx, 0, dz};
            const Vector3 x_line_end = {x_end, 0, dz};

            DrawLine(z_line_begin, z_line_end, x % 5 == 0 ? color_z : color_default);

            DrawLine(x_line_begin, x_line_end, z % 5 == 0 ? color_x : color_default);

            if (x % 5 == 0 && z % 5 == 0)
            {
                DrawLine({dx, 0, dz}, {dx, cam_y, dz}, color_y);
            }
        }
    }
}
}