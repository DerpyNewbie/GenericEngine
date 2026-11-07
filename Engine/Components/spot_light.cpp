#include "pch.h"
#include "game_object.h"
#include "spot_light.h"
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
}

CEREAL_REGISTER_TYPE(engine::SpotLight)
