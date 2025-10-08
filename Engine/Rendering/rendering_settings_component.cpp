#include "pch.h"
#include "rendering_settings_component.h"
#include "gui.h"
#include "render_pipeline.h"
#include "skybox.h"
#include "Components/directional_light.h"

namespace engine
{
bool RenderingSettingsComponent::ShadowCascadeInspector()
{
    return Gui::PropertyField("Cascade near", m_cascade_splits_[0]) ||
           Gui::PropertyField("Cascade mid", m_cascade_splits_[1]) ||
           Gui::PropertyField("Cascade far", m_cascade_splits_[2]);
}

void RenderingSettingsComponent::OnInspectorGui()
{
    bool is_changed = false;
    is_changed |= Gui::PropertyField("Skybox Cube", m_skybox_cube_);
    is_changed |= ShadowCascadeInspector();

    if (is_changed)
        ApplySettings();
}

void RenderingSettingsComponent::OnStart()
{
    ApplySettings();
}

void RenderingSettingsComponent::ApplySettings()
{
    Skybox::Instance()->SetTextureCube(m_skybox_cube_.CastedLock());
    RenderPipeline::SetCascadeSprits(m_cascade_splits_);
    DirectionalLight::SetCascadeSprits(m_cascade_splits_);
}
}