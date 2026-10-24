#include "pch.h"
#include "light.h"
#include "gui.h"
#include "Rendering/lighting.h"

namespace engine
{
void Light::OnInspectorGui()
{
    Gui::PropertyField("Intensity", m_light_data_.intensity);
    Gui::ColorField("LightColor", m_light_data_.color);

    // 影の割り当て（枚数が足りない場合は落とさない）はCaboRenderingSystemが毎フレーム行う
    bool cast_shadow = m_light_data_.cast_shadow != 0;
    if (Gui::BoolField("CastShadow", cast_shadow))
        m_light_data_.cast_shadow = cast_shadow ? 1 : 0;
}

void Light::OnEnabled()
{
    Lighting::Instance()->AddLight(shared_from_base<Light>());
}

void Light::OnDisabled()
{
    Lighting::Instance()->RemoveLight(shared_from_base<Light>());
}

void Light::OnDestroy()
{
    Lighting::Instance()->RemoveLight(shared_from_base<Light>());
}
}

CEREAL_REGISTER_TYPE(engine::Light)