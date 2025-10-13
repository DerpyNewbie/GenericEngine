#include "pch.h"
#include "directional_light.h"
#include "game_object.h"

std::array<float, engine::RenderingConstants::kShadowCascadeCount> engine::DirectionalLight::m_cascade_slices_;

using namespace DirectX;

namespace engine
{

void DirectionalLight::CascadeFrustum(const std::array<Vector3, 8> &frustum,
                                      std::array<std::array<Vector3, 8>,
                                                 RenderingConstants::kShadowCascadeCount> &dst)
{
    for (int i = 0; i < RenderingConstants::kShadowCascadeCount; ++i)
    {
        const auto camera = CameraComponent::Current();
        const auto camera_near = camera->m_property_.near_plane;
        const auto camera_far = camera->m_property_.far_plane;

        auto cascade_near = i == 0 ? camera_near : m_cascade_slices_[i - 1];
        auto cascade_far = m_cascade_slices_[i];
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
    const std::array<float, RenderingConstants::kShadowCascadeCount> shadow_cascade_sprits)
{
    m_cascade_slices_ = shadow_cascade_sprits;
}

void DirectionalLight::OnInspectorGui()
{
    Light::OnInspectorGui();
}

void DirectionalLight::OnUpdate()
{
    auto transform = GameObject()->Transform();
    Vector3 forward = transform->Forward();
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
    for (int i = 0; i < cascade_frustums.size(); ++i)
    {
        auto light_dir = Vector3(m_light_data_.direction.x, m_light_data_.direction.y, m_light_data_.direction.z);
        light_dir.Normalize();
        Vector3 up = XMVectorSet(0, 1, 0, 0);
        if (fabsf(XMVectorGetX(XMVector3Dot(up, light_dir))) > 0.9f)
            up = XMVectorSet(0, 0, 1, 0);

        Vector3 frustumCenter = XMVectorZero();
        for (auto &c : cascade_frustums[i])
            frustumCenter += c;
        frustumCenter /= static_cast<float>(frustum_corners.size());

        Vector3 lightPos = frustumCenter - light_dir * 1.0f;
        Matrix light_view = XMMatrixLookAtRH(lightPos, frustumCenter, up);

        Vector3 minPt = XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 1);
        Vector3 maxPt = XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 1);
        for (auto &c : cascade_frustums[i])
        {
            Vector3 v = XMVector3TransformCoord(c, light_view);
            minPt = XMVectorMin(minPt, v);
            maxPt = XMVectorMax(maxPt, v);
        }

        const float min_x = XMVectorGetX(minPt);
        const float min_y = XMVectorGetY(minPt);
        const float min_z = XMVectorGetZ(minPt);
        const float max_x = XMVectorGetX(maxPt);
        const float max_y = XMVectorGetY(maxPt);
        const float max_z = XMVectorGetZ(maxPt);

        const Matrix light_proj = XMMatrixOrthographicOffCenterRH(min_x, max_x, min_y, max_y, min_z, max_z);
        view_proj_matrices.emplace_back(light_view * light_proj);
    }

    return view_proj_matrices;
}
}