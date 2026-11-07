#include "pch.h"
#include "directional_light.h"
#include "game_object.h"
#include "Rendering/lighting.h"

namespace engine
{
void DirectionalLight::SetCascadeSlices(
    const std::array<float, RenderingConstants::kShadowCascadeCount> &shadow_cascade_slices)
{
    Lighting::Instance()->SetCascadeSlices(shadow_cascade_slices);
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
    m_light_data_.direction = transform->Forward();
}
}

CEREAL_REGISTER_TYPE(engine::DirectionalLight)
