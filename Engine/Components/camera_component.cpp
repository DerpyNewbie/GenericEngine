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

void CameraComponent::OnAwake()
{
    for (auto &view_proj_matrix_buffer : m_view_proj_matrix_buffers_)
    {
        view_proj_matrix_buffer = std::make_shared<ConstantBuffer>(sizeof(ViewProjection));
        view_proj_matrix_buffer->CreateBuffer();
    }
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

void CameraComponent::SetViewProjMatrix() const
{
    const auto cmd_list = RenderEngine::CommandList();
    const auto current_buffer_idx = RenderEngine::CurrentBackBufferIndex();
    const auto view_projection_buffer = m_view_proj_matrix_buffers_[current_buffer_idx];
    const auto view_projection = view_projection_buffer->GetPtr<ViewProjection>();

    view_projection->matrices[0] = ViewMatrix();
    view_projection->matrices[1] = ProjectionMatrix();

    cmd_list->SetGraphicsRootConstantBufferView(kViewProjCBV, view_projection_buffer->GetAddress());
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

std::vector<std::shared_ptr<Renderer>> CameraComponent::FilterVisibleObjects(
    const std::vector<std::shared_ptr<Renderer>> &renderers) const
{
    Matrix view_matrix = ViewMatrix();
    Matrix proj_matrix = ProjectionMatrix();

    DirectX::BoundingFrustum frustum;
    DirectX::BoundingFrustum::CreateFromMatrix(frustum, proj_matrix, true);
    frustum.Transform(frustum, view_matrix.Invert());

    std::vector<std::shared_ptr<Renderer>> results;
    for (auto renderer : renderers)
    {
        auto world_matrix = renderer->BoundsOrigin()->WorldMatrix();

        DirectX::BoundingBox world_bounds;
        renderer->bounds.Transform(world_bounds, world_matrix);

        if (frustum.Intersects(world_bounds))
        {
            results.emplace_back(renderer);
        }
    }
    return results;
}

Matrix CameraComponent::ViewMatrix() const
{
    auto transform = GameObject()->Transform();
    return DirectX::XMMatrixLookAtRH(transform->Position(), transform->Position() + transform->Forward(),
                                     transform->Up());
}

Matrix CameraComponent::ProjectionMatrix() const
{
    {
        switch (m_property_.view_mode)
        {
        case kViewMode::kPerspective:
            return DirectX::XMMatrixPerspectiveFovRH(
                m_property_.field_of_view * Mathf::kDeg2Rad,
                m_property_.aspect_ratio,
                m_property_.near_plane, m_property_.far_plane);
        case kViewMode::kOrthographic:
            return DirectX::XMMatrixOrthographicRH(
                m_property_.ortho_size * m_property_.aspect_ratio,
                m_property_.ortho_size,
                m_property_.near_plane, m_property_.far_plane);
        default:
            assert(false && "Invalid ViewMode");
            return Matrix::Identity;
        }
    }
}
}

CEREAL_REGISTER_TYPE(engine::CameraComponent)