#include "camera.h"

#include "DxLib.h"

namespace engine
{
void Camera::ApplyCameraSettingToDxLib()
{
    switch (m_view_mode_)
    {
    case kPerspective:
        SetupCamera_Perspective(m_field_of_view_);
        break;
    case kOrthographic:
        SetupCamera_Ortho(m_ortho_size_);
        break;
    }

    SetBackgroundColor(m_background_color_.r, m_background_color_.g, m_background_color_.b);
}

void Camera::OnUpdate()
{
    ApplyCameraSettingToDxLib();
}
}

CEREAL_REGISTER_TYPE(engine::Camera)

CEREAL_REGISTER_POLYMORPHIC_RELATION(engine::Component, engine::Camera)