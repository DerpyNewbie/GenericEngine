#include "pch.h"
#include "rendering_settings_component.h"

#include "gui.h"
#include "skybox.h"

namespace engine
{
void RenderingSettingsComponent::OnInspectorGui()
{
    if (Gui::PropertyField("Skybox Cube", m_skybox_cube_))
        ApplySettings();
}

void RenderingSettingsComponent::OnStart()
{
    ApplySettings();
}

void RenderingSettingsComponent::ApplySettings()
{
    Skybox::Instance()->SetTextureCube(m_skybox_cube_.CastedLock());
}
}