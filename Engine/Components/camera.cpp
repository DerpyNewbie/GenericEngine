#include "pch.h"

#include "camera.h"

#include <DirectXMath.h>
#include "DxLib/dxlib_converter.h"
#include "application.h"
#include "game_object.h"


namespace engine
{
std::weak_ptr<Camera> Camera::m_main_camera_;

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
    SetCameraNearFar(m_near_plane_, m_far_plane_);
    const auto transform = GameObject()->Transform();

    SetCameraPositionAndTargetAndUpVec(DxLibConverter::From(transform->Position()),
                                       DxLibConverter::From(transform->Position() + transform->Forward()),
                                       DxLibConverter::From(transform->Up()));
}
void Camera::OnAwake()
{
    m_main_camera_ = shared_from_base<Camera>();
}
void Camera::OnUpdate()
{
    ApplyCameraSettingToDxLib();
}
std::weak_ptr<Camera> Camera::Main()
{
    return m_main_camera_;
}
Matrix Camera::GetViewMatrix() const
{
    const auto transform = GameObject()->Transform();
    return DirectX::XMMatrixLookAtRH(transform->Position(), transform->Position() + transform->Forward(),
                                     transform->Up());
}
Matrix Camera::GetProjectionMatrix() const
{
    const float aspect = static_cast<float>(Application::WindowWidth() / Application::WindowHeight());
    return DirectX::XMMatrixPerspectiveFovRH(m_field_of_view_, aspect, 0.3f, 1000.0f);
}
}

CEREAL_REGISTER_TYPE(engine::Camera)