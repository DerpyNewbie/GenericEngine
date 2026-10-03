#include "pch.h"
#include "cinema_camera_transitioner.h"

#include "gui.h"

namespace engine
{
void CinemaCameraTransitioner::OnInspectorGui()
{
    Gui::PropertyField("FromCamera", m_from_camera_);
    Gui::PropertyField("ToCamera", m_to_camera_);
    Gui::PropertyField("CinemaBrain", m_cinema_brain_);

    if (ImGui::Button("Blend"))
    {
        m_cinema_brain_->Blend(m_from_camera_, m_to_camera_, 10, 0);
    }
}
}

CEREAL_REGISTER_TYPE(engine::CinemaCameraTransitioner)