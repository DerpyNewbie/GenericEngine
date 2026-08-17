#include "pch.h"
#include "light.h"
#include "gui.h"
#include "Rendering/lighting.h"
#include "Rendering/render_pipeline.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
void Light::OnInspectorGui()
{
    Gui::PropertyField("Intensity", m_light_data_.intensity);
    Gui::ColorField("LightColor", m_light_data_.color);

    if (Gui::BoolField("CastShadow", m_has_shadow_))
    {
        SetCastShadow(m_has_shadow_);
    }
}

void Light::OnDeserialized()
{
    SetCastShadow(m_has_shadow_);
}

void Light::OnEnabled()
{
    Lighting::Instance()->AddLight(shared_from_base<Light>());
    if (m_has_shadow_)
        Lighting::Instance()->TryApplyShadow(shared_from_base<Light>());
}

void Light::OnDisabled()
{
    Lighting::Instance()->RemoveLight(shared_from_base<Light>());
}

void Light::OnDestroy()
{
    Lighting::Instance()->RemoveLight(shared_from_base<Light>());
}

void Light::SetCastShadow(const bool cast_shadow)
{
    m_has_shadow_ = cast_shadow;
    if (m_has_shadow_ == false)
    {
        m_light_data_.cast_shadow = false;
        Lighting::Instance()->RemoveShadow(shared_from_base<Light>());
    }
    else
    {
        Lighting::Instance()->TryApplyShadow(shared_from_base<Light>());
    }
}
}

CEREAL_REGISTER_TYPE(engine::Light)