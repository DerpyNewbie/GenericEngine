#include "pch.h"
#include "blender.h"

#include "gui.h"

namespace engine
{
void Blender::OnInspectorGui()
{
    Gui::PropertyField("FromCamera", m_from_camera_);
    Gui::PropertyField("ToCamera", m_to_camera_);
    Gui::PropertyField("CinemaBrain", m_cinema_brain_);

    if (ImGui::Button("Blend"))
    {
        m_cinema_brain_.CastedLock()->Blend(m_from_camera_.CastedLock(), m_to_camera_.CastedLock(), 10, 0);
    }
}
}