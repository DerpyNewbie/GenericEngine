#include "pch.h"
#include "game_object.h"
#include "spot_light.h"

#include "camera_component.h"
#include "gui.h"

void engine::SpotLight::OnAwake()
{
    m_light_data_.type = static_cast<int>(kLightType::kSpotLight);
    m_light_data_.inner_cos = cos(DirectX::XMConvertToRadians(50.0f));
    m_light_data_.outer_cos = cos(DirectX::XMConvertToRadians(45.0f));
}
void engine::SpotLight::OnInspectorGui()
{
    Gui::PropertyField("InnerAngle", m_light_data_.inner_cos);
    Gui::PropertyField("OuterAngle", m_light_data_.outer_cos);
    Gui::PropertyField("Range", m_light_data_.range);
    Light::OnInspectorGui();
}
void engine::SpotLight::OnUpdate()
{
    auto transform = GameObject()->Transform();

    Vector3 position = transform->Position();
    m_light_data_.pos.x = position.x;
    m_light_data_.pos.y = position.y;
    m_light_data_.pos.z = position.z;

    Vector3 forward = transform->Forward();
    m_light_data_.direction.x = forward.x;
    m_light_data_.direction.y = forward.y;
    m_light_data_.direction.z = forward.z;
}

bool engine::SpotLight::InCameraView(const std::array<Vector3, 8> &frustum)
{
    float cone_length = m_light_data_.range;
    float cone_radius = tanf(acosf(m_light_data_.outer_cos)) * cone_length;

    // 球の中心を照射方向に 0.5 * range 移動した位置に置く
    auto light_pos = Vector3(m_light_data_.pos.x, m_light_data_.pos.y, m_light_data_.pos.z);
    auto light_dir = Vector3(m_light_data_.direction.x, m_light_data_.direction.y, m_light_data_.direction.z);
    light_dir.Normalize();
    Vector3 sphere_center = light_pos + light_dir * (cone_length * 0.5f);

    // 半径は照射範囲の半分程度
    float sphere_radius = 0.5f * sqrtf(cone_length * cone_length + cone_radius * cone_radius);

    Vector3 min_v(FLT_MAX);
    Vector3 max_v(-FLT_MAX);

    for (const auto &v : frustum)
    {
        min_v.x = min(min_v.x, v.x);
        min_v.y = min(min_v.y, v.y);
        min_v.z = min(min_v.z, v.z);

        max_v.x = max(max_v.x, v.x);
        max_v.y = max(max_v.y, v.y);
        max_v.z = max(max_v.z, v.z);
    }

    float x = max(min_v.x, min(sphere_center.x, max_v.x));
    float y = max(min_v.y, min(sphere_center.y, max_v.y));
    float z = max(min_v.z, min(sphere_center.z, max_v.z));

    // 最近点との距離で判定
    Vector3 nearest(x, y, z);
    float distSq = (sphere_center - nearest).LengthSquared();
    return distSq <= sphere_radius * sphere_radius;
}

Vector3 engine::SpotLight::GetPos()
{
    return GameObject()->Transform()->Position();
}

int engine::SpotLight::ShadowMapCount()
{
    return 1;
}

std::vector<Matrix> engine::SpotLight::CalcViewProj(const std::array<Vector3, 8> &frustum_corners)
{
    auto transform = GameObject()->Transform();
    Matrix view = DirectX::XMMatrixLookAtRH(transform->Position(),
                                            transform->Position() + transform->Forward(),
                                            DirectX::XMVectorSet(0, 1, 0, 0));
    auto outer_angle = acosf(m_light_data_.outer_cos);
    Matrix proj = DirectX::XMMatrixPerspectiveFovRH(outer_angle == 0 ? 0.1f : outer_angle * 2.0f, 1.0f,
                                                    CameraComponent::Current()->m_property_.near_plane,
                                                    m_light_data_.range);
    return {view * proj};
}