#include "pch.h"
#include "global_resource_tester_component.h"

#include "gui.h"
#include "Rendering/gpu_resource_manager.h"

namespace engine
{
void GlobalResourceTesterComponent::OnInspectorGui()
{
    if (Gui::PropertyField("_MainTex", m_texture_))
    {
        GpuResourceManager::SetGlobalTexture("_MainTex", m_texture_);
    }
}
void GlobalResourceTesterComponent::OnDeserialized()
{
    GpuResourceManager::SetGlobalTexture("_MainTex", m_texture_);
}
}

CEREAL_REGISTER_TYPE(engine::GlobalResourceTesterComponent)