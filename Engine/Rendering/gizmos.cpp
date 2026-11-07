#include "pch.h"
#include "gizmos.h"

namespace engine
{
std::vector<Gizmos::LineVertex> Gizmos::m_vertices_;

void Gizmos::Init()
{}

std::vector<Gizmos::LineVertex> Gizmos::TakeLines()
{
    return std::exchange(m_vertices_, {});
}

void Gizmos::DrawLine(const Vector3 &start, const Vector3 &end, const Color &color)
{
    m_vertices_.push_back({start, color});
    m_vertices_.push_back({end, color});
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
