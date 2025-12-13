#include "pch.h"
#include "game_object.h"
#include "spot_light.h"

#include "camera_component.h"
#include "gui.h"

namespace engine
{
void SpotLight::OnConstructed()
{
    Light::OnConstructed();
    m_light_data_.type = static_cast<int>(kLightType::kSpotLight);
    m_light_data_.inner_cos = std::cos(DirectX::XMConvertToRadians(45.0f));
    m_light_data_.outer_cos = std::cos(DirectX::XMConvertToRadians(50.0f));
}

void SpotLight::OnInspectorGui()
{
    Gui::PropertyField("Inner Angle", m_light_data_.inner_cos);
    Gui::PropertyField("Outer Angle", m_light_data_.outer_cos);
    Gui::PropertyField("Range", m_light_data_.range);
    Light::OnInspectorGui();
}

void SpotLight::UpdateData()
{
    const auto transform = GameObject()->Transform();
    m_light_data_.pos = transform->Position();
    m_light_data_.direction = transform->Forward();
}

bool SpotLight::InCameraView(const std::array<Vector3, 8> &frustum)
{
    const float cone_length = m_light_data_.range;
    const float cone_radius = std::tanf(std::acosf(m_light_data_.outer_cos)) * cone_length;

    // 球の中心を照射方向に 0.5 * range 移動した位置に置く
    const auto light_pos = m_light_data_.pos;
    auto light_dir = m_light_data_.direction;
    light_dir.Normalize();
    const Vector3 sphere_center = light_pos + light_dir * (cone_length * 0.5f);

    // 半径は照射範囲の半分程度
    const float sphere_radius = 0.5f * std::sqrtf(cone_length * cone_length + cone_radius * cone_radius);

    Vector3 min_v(FLT_MAX);
    Vector3 max_v(-FLT_MAX);

    for (const auto &v : frustum)
    {
        Vector3::Min(min_v, v, min_v);
        Vector3::Max(max_v, v, max_v);
    }

    // 最近点との距離で判定
    const Vector3 nearest = Vector3::Max(min_v, Vector3::Min(sphere_center, max_v));
    const float distance_sq = (sphere_center - nearest).LengthSquared();
    return distance_sq <= sphere_radius * sphere_radius;
}

Vector3 SpotLight::GetPos()
{
    return GameObject()->Transform()->Position();
}

int SpotLight::ShadowMapCount()
{
    return 1;
}

std::vector<Matrix> SpotLight::CalcViewProj(const std::array<Vector3, 8> &frustum_corners)
{
    const auto transform = GameObject()->Transform();
    const Matrix view = Matrix::CreateLookAt(
        transform->Position(),
        transform->Position() + transform->Forward(),
        Vector3::Up
    );

    const auto outer_angle = acosf(m_light_data_.outer_cos);
    const Matrix proj = Matrix::CreatePerspective(
        Mathf::Approximately(outer_angle, 0) ? 0.1f : outer_angle * 2.0f,
        1.0f,
        CameraComponent::Current()->m_property_.near_plane,
        m_light_data_.range
    );

    return { view * proj };
}
}

CEREAL_REGISTER_TYPE(engine::SpotLight)