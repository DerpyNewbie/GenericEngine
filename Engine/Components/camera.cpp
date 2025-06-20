#include "pch.h"

#include "camera.h"

#include <DirectXMath.h>
#include "DxLib/dxlib_converter.h"
#include "application.h"
#include "game_object.h"
#include "Math/mathf.h"


namespace engine
{
std::weak_ptr<Camera> Camera::m_main_camera_;

void Camera::ApplyCameraSettingToDxLib() const
{
    SetBackgroundColor(m_background_color_.R() * 255, m_background_color_.G() * 255, m_background_color_.B() * 255);
    const auto t = GameObject()->Transform();

    int x, y;
    GetDrawScreenSize(&x, &y);
    const float aspect = static_cast<float>(x) / static_cast<float>(y);
    const auto proj = m_view_mode_ == kViewMode::kPerspective
                          ? Matrix::CreatePerspectiveFieldOfView(m_field_of_view_ * Mathf::kDeg2Rad, aspect,
                                                                 m_near_plane_, m_far_plane_)
                          : Matrix::CreateOrthographic(m_ortho_size_, m_ortho_size_, m_near_plane_, m_far_plane_);
    const auto view = Matrix::CreateLookAt(t->Position(), t->Position() + t->Forward(), t->Up());

    SetupCamera_ProjectionMatrix(DxLibConverter::From(proj));
    SetCameraViewMatrix(DxLibConverter::From(view));
}
void Camera::OnAwake()
{
    m_main_camera_ = shared_from_base<Camera>();
}
void Camera::OnUpdate()
{
    ApplyCameraSettingToDxLib();
}
void Camera::OnInspectorGui()
{
    ImGui::SliderFloat("Field of View", &m_field_of_view_,
                       min_field_of_view, max_field_of_view);

    ImGui::SliderFloat("Near Plane", &m_near_plane_,
                       min_clipping_plane, Mathf::Min(m_far_plane_, max_clipping_plane));

    ImGui::SliderFloat("Far Plane", &m_far_plane_,
                       Mathf::Max(m_near_plane_, min_clipping_plane) + 0.1F, max_clipping_plane);

    ImGui::InputFloat("Ortho Size", &m_ortho_size_);
    m_ortho_size_ = Mathf::Max(m_ortho_size_, 0.1F);

    int current_view_mode = static_cast<int>(m_view_mode_);
    if (ImGui::Combo("View Mode", &current_view_mode, "Perspective\0Orthographic\0\0"))
    {
        m_view_mode_ = static_cast<kViewMode>(current_view_mode);
    }

    float color_buf[4];
    EngineUtil::ToFloat4(color_buf, m_background_color_);
    if (ImGui::ColorPicker4("Background Color", color_buf))
    {
        m_background_color_ = Color{color_buf[0], color_buf[1], color_buf[2], color_buf[3]};
    }
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