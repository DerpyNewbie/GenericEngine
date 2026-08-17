#include "pch.h"
#include "rendering_settings_component.h"
#include "gui.h"
#include "lighting.h"
#include "render_pipeline.h"
#include "skybox.h"
#include "Components/directional_light.h"

namespace engine
{
bool RenderingSettingsComponent::ShadowCascadeInspector()
{
    return Gui::PropertyField("Cascade near", m_cascade_slices_[0]) ||
           Gui::PropertyField("Cascade mid", m_cascade_slices_[1]) ||
           Gui::PropertyField("Cascade far", m_cascade_slices_[2]);
}

void RenderingSettingsComponent::OnInspectorGui()
{
    bool has_changed = false;
    has_changed |= Gui::PropertyField("Skybox Cube", m_skybox_cube_);
    has_changed |= ShadowCascadeInspector();

    ImGui::Checkbox("Apply Settings", &has_changed);
    
    if (has_changed)
        ApplySettings();
}
void RenderingSettingsComponent::OnAwake()
{
    ApplySettings();
}

void RenderingSettingsComponent::ApplySettings()
{
    Skybox::Instance()->SetTextureCube(m_skybox_cube_.CastedLock());
    Lighting::Instance()->SetCascadeSlices(m_cascade_slices_);
    DirectionalLight::SetCascadeSlices(m_cascade_slices_);
}
}

CEREAL_REGISTER_TYPE(engine::RenderingSettingsComponent)