#include "pch.h"

#include "gui.h"
#include "application.h"
#include "Components/camera_component.h"

#include "update_manager.h"
#include "Rendering/render_pipeline.h"

namespace engine
{
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
    switch (view_mode)
    {
    case kViewMode::kPerspective:
        return DirectX::XMMatrixPerspectiveFovRH(
            field_of_view * Mathf::kDeg2Rad,
            aspect_ratio,
            near_plane,
            far_plane
        );
    case kViewMode::kOrthographic:
        return DirectX::XMMatrixOrthographicRH(
            ortho_size * aspect_ratio,
            ortho_size,
            near_plane,
            far_plane
        );
    default:
        assert(false && "Invalid ViewMode");
        return Matrix::Identity;
    }
}

void CameraComponent::OnInspectorGui()
{
    m_rendering_layer_.OnInspectorGui();
    property.OnInspectorGui();
    Gui::PropertyField("Render Texture", m_render_texture_);
    Gui::PropertyField("Depth Texture", m_depth_texture_);

    if (ImGui::Button("Set Main Camera"))
        SetMainCamera(shared_from_base<CameraComponent>());
}

void CameraComponent::OnValidate()
{
    if (GameObject()->IsActiveInHierarchy())
    {
        OnEnabled();
    }
    else
    {
        OnDisabled();
    }
}

void CameraComponent::Render()
{
    RenderPipeline::RequestRender(GetCamera());
}

void CameraComponent::OnEnabled()
{
    if (Main() == nullptr)
    {
        SetMainCamera(shared_from_base<CameraComponent>());
    }

    const auto shared_this = shared_from_base<CameraComponent>();
    const auto find = std::ranges::find(
        m_cameras_,
        shared_this,
        [](auto a)
        {
            return a.lock();
        }
    );

    if (find == m_cameras_.end())
        m_cameras_.emplace_back(shared_from_base<CameraComponent>());

    UpdateManager::SubscribeRender(shared_from_base<IRenderReceiver>());
}

void CameraComponent::OnDisabled()
{
    auto shared_this = shared_from_base<CameraComponent>();
    std::erase_if(
        m_cameras_,
        [shared_this](const std::weak_ptr<CameraComponent>& a)
        {
            return a.expired() || a.lock() == shared_this;
        }
    );

    if (m_main_camera_.lock() == shared_this)
    {
        SetMainCamera(m_cameras_.begin() != m_cameras_.end() ? *m_cameras_.begin() : std::weak_ptr<CameraComponent>());
    }

    UpdateManager::UnsubscribeRender(shared_from_base<IRenderReceiver>());
}

Camera CameraComponent::GetCamera()
{
    return Camera(reinterpret_cast<UINT64>(shared_from_base<CameraComponent>().get()), m_rendering_layer_,
                  property.background_color, ViewMatrix(), property.ProjectionMatrix(), m_render_texture_.CastedLock(),
                  m_depth_texture_.CastedLock());
}

Layer CameraComponent::GetRenderingLayer() const
{
    return m_rendering_layer_;
}

std::shared_ptr<RenderTexture> CameraComponent::GetRenderTexture()
{
    return m_render_texture_.CastedLock() != nullptr
               ? m_render_texture_.CastedLock()
               : (m_render_texture_ = AssetPtr<class RenderTexture>::FromInstance(Instantiate<class RenderTexture>()))
               .CastedLock();
}

Vector3 CameraComponent::ScreenPosToWorldPos(Vector2 screen_pos, float z_pos) const
{
    const auto view_port = RenderEngine::Viewport();
    const Vector3 screen_near = {screen_pos.x, screen_pos.y, 0.0f};

    const Vector3 ray_origin = DirectX::XMVector3Unproject(
        screen_near,
        0.0f, 0.0f, view_port.Width, view_port.Height,
        0.0f, 1.0f,
        property.ProjectionMatrix(), ViewMatrix(), DirectX::XMMatrixIdentity()
    );

    const Vector3 screen_far = {screen_pos.x, screen_pos.y, 1.0f};

    Vector3 ray_dest = DirectX::XMVector3Unproject(
        screen_far,
        0.0f, 0.0f, view_port.Width, view_port.Height,
        0.0f, 1.0f,
        property.ProjectionMatrix(), ViewMatrix(), DirectX::XMMatrixIdentity()
    );

    const auto ray_dir = ray_dest - ray_origin;

    Vector3 normalized_dir;
    ray_dir.Normalize(normalized_dir);

    const Vector3 camera_forward = GameObject()->Transform()->Forward();
    float dot = normalized_dir.Dot(camera_forward);

    if (Mathf::Approximately(dot, 0.0f))
    {
        return ray_origin + (normalized_dir * z_pos);
    }

    const float forward_distance = z_pos / dot;
    return (normalized_dir * forward_distance);
}

Vector2 CameraComponent::WorldPosToScreenPos(const Vector3 world_pos) const
{
    const auto view_port = RenderEngine::Viewport();

    const Vector3 screen_pos = DirectX::XMVector3Project(
        world_pos,
        0.0f, 0.0f, view_port.Width, view_port.Height,
        0.0f, 1.0f,
        property.ProjectionMatrix(), ViewMatrix(), DirectX::XMMatrixIdentity()
    );

    return {screen_pos.x, screen_pos.y};
}

void CameraComponent::SetRenderTexture(const AssetPtr<RenderTexture>& render_texture)
{
    m_render_texture_ = render_texture;
}

void CameraComponent::SetMainCamera(const std::weak_ptr<CameraComponent>& camera)
{
    m_main_camera_ = camera;
}

std::shared_ptr<CameraComponent> CameraComponent::Main()
{
    auto camera = m_main_camera_.lock();
    return camera != nullptr && !camera->IsDestroying() ? camera : nullptr;
}


Matrix CameraComponent::ViewMatrix() const
{
    const auto transform = GameObject()->Transform();
    if (transform == nullptr)
    {
        return DirectX::XMMatrixLookAtRH(Vector3::Zero, Vector3::Forward, Vector3::Up);
    }

    return DirectX::XMMatrixLookAtRH(
        transform->Position(),
        transform->Position() + transform->Forward(),
        transform->Up()
    );
}
}

CEREAL_REGISTER_TYPE(engine::CameraComponent)
