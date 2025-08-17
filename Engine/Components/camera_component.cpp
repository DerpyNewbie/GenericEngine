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
    Matrix view_matrix = GetViewMatrix();
    Matrix proj_matrix = GetProjectionMatrix();

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
    return {};
}

int CameraComponent::Order()
{
    return Main() == shared_from_base<Camera>() ? INT_MAX - 20000 : INT_MAX - 30000;
}

void CameraComponent::OnAwake()
{
    if (Main() == nullptr)
    {
        SetMainCamera(shared_from_base<Camera>());
    }
}

void CameraComponent::OnInspectorGui()
{
    m_property_.OnInspectorGui();
    Gui::PropertyField("RenderTexture", m_render_texture_);
    ImGui::Text("DrawCall Count:%d", m_drawcall_count_);
}

void CameraComponent::OnDraw()
{
    SetTransform(GameObject()->Transform());
    SetCurrentCamera(shared_from_base<Camera>());
    auto objects_in_view = FilterVisibleObjects(Renderer::m_renderers_);
    if (auto render_tex = m_render_texture_.CastedLock())
    {
        render_tex->BeginRender(m_property_.background_color);
        Render(objects_in_view);
    }
    if (Main() == shared_from_base<Camera>())
    {
        g_RenderEngine->SetMainRenderTarget(m_property_.background_color);
        Render(objects_in_view);
    }
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