#include "pch.h"
#include "camera_component.h"
#include "directional_light.h"
#include "game_object.h"

std::array<float, engine::RenderingConstants::kShadowCascadeCount> engine::DirectionalLight::m_cascade_slices_;

using namespace DirectX;

namespace engine
{
void DirectionalLight::CascadeFrustum(
    const std::array<Vector3, 8> &frustum,
    std::array<std::array<Vector3, 8>, RenderingConstants::kShadowCascadeCount> &dst)
{
    for (int i = 0; i < RenderingConstants::kShadowCascadeCount; ++i)
    {
        const auto camera = CameraComponent::Current();
        const auto camera_near = camera->m_property_.near_plane;
        const auto camera_far = camera->m_property_.far_plane;

        const auto cascade_near = i == 0 ? camera_near : m_cascade_slices_[i - 1];
        const auto cascade_far = m_cascade_slices_[i];
        const float t_near = (cascade_near - camera_near) / (camera_far - camera_near);
        const float t_far = (cascade_far - camera_near) / (camera_far - camera_near);

        for (int j = 0; j < 4; j++)
        {
            Vector3 near_corner = frustum[j];
            Vector3 far_corner = frustum[j + 4];

            dst[i][j] = XMVectorLerp(near_corner, far_corner, t_near);
            dst[i][j + 4] = XMVectorLerp(near_corner, far_corner, t_far);
        }
    }
}

void DirectionalLight::SetCascadeSlices(
    const std::array<float, RenderingConstants::kShadowCascadeCount> shadow_cascade_slices)
{
    m_cascade_slices_ = shadow_cascade_slices;
}

void DirectionalLight::OnConstructed()
{
    Light::OnConstructed();
    m_light_data_.type = static_cast<int>(kLightType::kDirectional);
}

void DirectionalLight::OnInspectorGui()
{
    Light::OnInspectorGui();
}

void DirectionalLight::UpdateData()
{
    const auto transform = GameObject()->Transform();
    const Vector3 forward = transform->Forward();
    m_light_data_.direction.x = forward.x;
    m_light_data_.direction.y = forward.y;
    m_light_data_.direction.z = forward.z;
}

bool DirectionalLight::InCameraView(const std::array<Vector3, 8> &frustum)
{
    return true;
}
Vector3 DirectionalLight::GetPos()
{
    return CameraComponent::Main()->GameObject()->Transform()->Position();
}

int DirectionalLight::ShadowMapCount()
{
    return 3;
}

std::vector<Matrix> DirectionalLight::CalcViewProj(const std::array<Vector3, 8> &frustum_corners)
{
    std::vector<Matrix> view_proj_matrices;
    std::array<std::array<Vector3, 8>, RenderingConstants::kShadowCascadeCount> cascade_frustums;
    CascadeFrustum(frustum_corners, cascade_frustums);
    for (auto &cascade_corners : cascade_frustums)
    {
        Vector3 frustum_center{};
        for (const auto &corner : cascade_corners)
            frustum_center += corner;
        frustum_center /= static_cast<float>(frustum_corners.size());

        // check if a light direction is pointing upwards or downwards.
        // light_view needs up directions to be `light_dir !=(nearly) direction`.
        Vector3 light_dir = m_light_data_.direction;
        Vector3 up = Vector3::Up;
        if (std::fabsf(up.Dot(light_dir)) > 0.9f)
            up = Vector3::Forward;

        Vector3 light_pos = frustum_center - light_dir;
        Matrix light_view = Matrix::CreateLookAt(light_pos, frustum_center, up);

        // calculate min/max of cascade corners
        Vector3 min_corner(FLT_MAX);
        Vector3 max_corner(-FLT_MAX);
        for (auto &corner : cascade_corners)
        {
            Vector3 v = Vector3::Transform(corner, light_view);
            Vector3::Min(min_corner, v, min_corner);
            Vector3::Max(max_corner, v, max_corner);
        }

        const Matrix light_proj = Matrix::CreateOrthographicOffCenter(
            min_corner.x, max_corner.x,
            min_corner.y, max_corner.y,
            min_corner.z, max_corner.z
        );

        view_proj_matrices.emplace_back(light_view * light_proj);
    }

    return view_proj_matrices;
}
}

CEREAL_REGISTER_TYPE(engine::DirectionalLight)