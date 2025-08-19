#include "pch.h"

#include "gui.h"
#include "application.h"
#include "update_manager.h"
#include "Components/camera_component.h"

namespace engine
{

std::vector<std::shared_ptr<Renderer>> CameraComponent::FilterVisibleObjects(
    const std::vector<std::weak_ptr<Renderer>> &renderers) const
{
    Matrix view_matrix = m_camera_->ViewMatrix();
    Matrix proj_matrix = m_camera_->ProjectionMatrix();

    DirectX::BoundingFrustum frustum;
    DirectX::BoundingFrustum::CreateFromMatrix(frustum, proj_matrix, true);
    frustum.Transform(frustum, view_matrix.Invert());

    std::vector<std::shared_ptr<Renderer>> results;
    for (auto weak_renderer : renderers)
    {
        auto renderer = weak_renderer.lock();
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

int CameraComponent::Order()
{
    return Camera::Main() == m_camera_ ? INT_MAX - 20000 : INT_MAX - 30000;
}

void CameraComponent::OnAwake()
{
    m_camera_ = std::make_shared<Camera>();
    if (Camera::Main() == nullptr)
    {
        Camera::SetMainCamera(m_camera_);
    }
}

void CameraComponent::OnInspectorGui()
{
    m_camera_->m_property_.OnInspectorGui();
    Gui::PropertyField("RenderTexture", m_camera_->m_render_texture_);
    ImGui::Text("DrawCall Count:%d", m_drawcall_count_);
}

void CameraComponent::OnDraw()
{
    m_camera_->SetTransform(GameObject()->Transform());
    Camera::SetCurrentCamera(shared_from_base<Camera>());
    auto objects_in_view = FilterVisibleObjects(Renderer::m_renderers_);
    m_camera_->Render(objects_in_view);
}

void CameraComponent::OnEnabled()
{
    UpdateManager::SubscribeDrawCall(shared_from_base<CameraComponent>());
}

void CameraComponent::OnDisabled()
{
    UpdateManager::UnsubscribeDrawCall(shared_from_base<CameraComponent>());
}
}

CEREAL_REGISTER_TYPE(engine::CameraComponent)