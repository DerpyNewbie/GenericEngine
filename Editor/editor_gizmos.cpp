#include "pch.h"
#include "editor_gizmos.h"

#include "Rendering/gizmos.h"

namespace editor
{
void EditorGizmos::DrawYPlaneGrid(const Matrix &view_matrix, const Vector2 &spacing, const int count)
{
    const auto [cam_x, cam_y, cam_z] = Vector3::Transform(Vector3::Zero, view_matrix);
    const int half_count = count / 2;
    const int center_x_index = static_cast<int>(cam_x / spacing.x);
    const int center_z_index = static_cast<int>(cam_z / spacing.y);
    const int max_x = center_x_index + half_count;
    const int min_x = center_x_index - half_count;
    const int max_z = center_z_index + half_count;
    const int min_z = center_z_index - half_count;

    const float x_end = spacing.x * static_cast<float>(min_x);
    const float z_end = spacing.y * static_cast<float>(min_z);

    constexpr auto color_default = Color(0.6F, 0.6F, 0.6F, 1.0F);
    constexpr auto color_z = Color(0.6F, 0.6F, 1.0F, 1.0F);
    constexpr auto color_x = Color(1.0F, 0.6F, 0.6F, 1.0F);
    constexpr auto color_y = Color(0.6F, 1.0F, 0.6F, 1.0F);

    for (int z = min_z; z < max_z; ++z)
    {
        for (int x = min_x; x < max_x; ++x)
        {
            const auto dx = spacing.x * static_cast<float>(x);
            const auto dz = spacing.y * static_cast<float>(z);

            const Vector3 z_line_begin = {dx, 0, dz};
            const Vector3 z_line_end = {dx, 0, z_end};

            const Vector3 x_line_begin = {dx, 0, dz};
            const Vector3 x_line_end = {x_end, 0, dz};

            engine::Gizmos::DrawLine(z_line_begin, z_line_end, x % 5 == 0 ? color_z : color_default);

            engine::Gizmos::DrawLine(x_line_begin, x_line_end, z % 5 == 0 ? color_x : color_default);

            if (x % 5 == 0 && z % 5 == 0)
            {
                engine::Gizmos::DrawLine({dx, 0, dz}, {dx, cam_y, dz}, color_y);
            }
        }
    }
}

void EditorGizmos::DrawObject(const std::shared_ptr<engine::GameObject> &game_object)
{
    auto world_matrix = game_object->Transform()->WorldMatrix();
    Vector3 pos, sca;
    Quaternion rot;
    world_matrix.Decompose(sca, rot, pos);

    engine::Gizmos::DrawSphere(pos, 1, Color{1, 1, 1});
}
}