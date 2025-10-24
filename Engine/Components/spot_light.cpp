#include "pch.h"
#include "game_object.h"
#include "spot_light.h"

#include "camera_component.h"
#include "gui.h"

void engine::SpotLight::OnAwake()
{
    m_light_data_.type = static_cast<int>(kLightType::kSpotLight);
    m_light_data_.inner_cos = DirectX::XMConvertToRadians(50.0f);
    m_light_data_.outer_cos = DirectX::XMConvertToRadians(45.0f);
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
    return true;
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