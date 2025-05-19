#include "camera.h"

#include "DxLib.h"
#include "game_object.h"

namespace engine
{
void Camera::ApplyCameraSettingToDxLib() const
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
    const auto transform = GameObject()->Transform();
    SetCameraPositionAndTargetAndUpVec(transform->Position(), transform->Forward(), transform->Up());
}

void Camera::OnUpdate()
{
    ApplyCameraSettingToDxLib();
}
}

CEREAL_REGISTER_TYPE(engine::Camera)
