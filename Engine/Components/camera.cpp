#include "pch.h"

#include "camera.h"

#include "DxLib/dxlib_converter.h"
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

    int x, y;
    GetDrawScreenSize(&x, &y);

    const float aspect = static_cast<float>(x) / static_cast<float>(y);
    const auto projection = Matrix::CreatePerspectiveFieldOfView(m_field_of_view_, aspect, m_near_plane_, m_far_plane_);
    const auto view = Matrix::CreateLookAt(transform->Position(), transform->Position() + transform->Forward(),
                                           transform->Up());

    SetupCamera_ProjectionMatrix(DxLibConverter::From(projection));
    SetCameraViewMatrix(DxLibConverter::From(view));
}

void Camera::OnUpdate()
{
    ApplyCameraSettingToDxLib();
}
}

CEREAL_REGISTER_TYPE(engine::Camera)