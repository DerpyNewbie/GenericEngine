#include "pch.h"

#include "gui.h"
#include "application.h"
#include "Components/camera_component.h"

#include "Rendering/render_pipeline.h"
#include "Rendering/view_projection.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{

std::weak_ptr<CameraComponent> CameraComponent::m_main_camera_;
std::weak_ptr<CameraComponent> CameraComponent::m_current_camera_;

void CameraProperty::OnInspectorGui()
{
    ImGui::SliderFloat("Field of View", &field_of_view, kMinFieldOfView, kMaxFieldOfView);
    ImGui::SliderFloat("Near Plane", &near_plane, kMinClippingPlane, Mathf::Min(far_plane, kMaxClippingPlane));
    ImGui::SliderFloat("Far Plane", &far_plane, Mathf::Max(near_plane, kMinClippingPlane) + 0.1F, kMaxClippingPlane);

    if (Gui::PropertyField("Ortho Size", ortho_size))
    {
        ortho_size = Mathf::Max(ortho_size, 0.1F);
    }

    int current_view_mode = static_cast<int>(view_mode);
    if (ImGui::Combo("View Mode", &current_view_mode, "Perspective\0Orthographic\0\0"))
    {
        view_mode = static_cast<kViewMode>(current_view_mode);
    }

    Gui::PropertyField("Background Color", background_color);
}

Matrix CameraProperty::ProjectionMatrix() const
{
    {
        switch (view_mode)
        {
        case kViewMode::kPerspective:
            return DirectX::XMMatrixPerspectiveFovRH(
                field_of_view * Mathf::kDeg2Rad,
                aspect_ratio,
                near_plane, far_plane);
        case kViewMode::kOrthographic:
            return DirectX::XMMatrixOrthographicRH(
                ortho_size * aspect_ratio,
                ortho_size,
                near_plane, far_plane);
        default:
            assert(false && "Invalid ViewMode");
            return Matrix::Identity;
        }
    }
}

void CameraComponent::OnAwake()
{
    if (Main() == nullptr)
    {
        SetMainCamera(shared_from_base<CameraComponent>());
    }
}

void CameraComponent::OnInspectorGui()
{
    m_property_.OnInspectorGui();
    Gui::PropertyField("RenderTexture", m_render_texture_);
    ImGui::Text("DrawCall Count:%d", m_drawcall_count_);
}

void CameraComponent::OnEnabled()
{
    RenderPipeline::AddCamera(shared_from_base<CameraComponent>());
}

void CameraComponent::OnDisabled()
{
    RenderPipeline::RemoveCamera(shared_from_base<CameraComponent>());
}

void CameraComponent::SetMainCamera(const std::weak_ptr<CameraComponent> &camera)
{
    m_main_camera_ = camera;
}

void CameraComponent::SetCurrentCamera(const std::weak_ptr<CameraComponent> &camera)
{
    m_current_camera_ = camera;
}

std::shared_ptr<CameraComponent> CameraComponent::Main()
{
    return m_main_camera_.lock();
}

std::shared_ptr<CameraComponent> CameraComponent::Current()
{
    return m_current_camera_.lock();
}

Matrix CameraComponent::ViewMatrix() const
{
    auto transform = GameObject()->Transform();
    return DirectX::XMMatrixLookAtRH(transform->Position(), transform->Position() + transform->Forward(),
                                     transform->Up());
}
}

CEREAL_REGISTER_TYPE(engine::CameraComponent)