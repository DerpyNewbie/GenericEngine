#include "pch.h"

#include "camera.h"
#include "application.h"
#include "game_object.h"
#include "gui.h"
#include "update_manager.h"
#include "Rendering/gizmos.h"
#include "Rendering/view_projection.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"
#include "Rendering/CabotEngine/Graphics/DescriptorHeap.h"

namespace engine
{
std::weak_ptr<Camera> Camera::m_main_camera_;
std::weak_ptr<Camera> Camera::m_current_camera_;

bool Camera::BeginRender()
{
    if (Main() == shared_from_base<Camera>())
    {
        g_RenderEngine->SetMainRenderTarget();
        return true;
    }
    if (auto render_tex = render_texture.CastedLock())
    {
        render_tex->BeginRender();
        return true;
    }
    return false;
}

void Camera::EndRender()
{
    if (Main() == shared_from_base<Camera>())
    {
        return;
    }
    if (auto render_tex = render_texture.CastedLock())
    {
        render_tex->EndRender();
    }
}

void Camera::SetViewProjMatrix()
{
    for (auto view_proj_matrix_buffer : m_view_proj_matrix_buffers_)
    {
        auto ptr = view_proj_matrix_buffer->GetPtr<ViewProjection>();
        ptr->matrices[0] = GetViewMatrix();
        ptr->matrices[1] = GetProjectionMatrix();
    }
    auto cmd_list = g_RenderEngine->CommandList();
    auto current_buffer = g_RenderEngine->CurrentBackBufferIndex();
    cmd_list->SetGraphicsRootConstantBufferView(kViewProjCBV,
                                                m_view_proj_matrix_buffers_[current_buffer]->GetAddress());
}

std::vector<std::shared_ptr<Renderer>> Camera::FilterVisibleObjects(
    const std::vector<std::weak_ptr<Renderer>> &renderers)
{
    Matrix view_matrix = GetViewMatrix();
    Matrix proj_matrix = GetProjectionMatrix();
    DirectX::BoundingFrustum frustum;
    DirectX::BoundingFrustum::CreateFromMatrix(frustum, proj_matrix, true);
    frustum.Transform(frustum, view_matrix.Invert());

    std::vector<std::shared_ptr<Renderer>> visible_objects;
    for (auto renderer : renderers)
    {
        auto transform = renderer.lock()->BoundsOrigin();
        auto world_matrix = transform.lock()->WorldMatrix();
        auto bounds = renderer.lock()->bounds;
        auto min_pos = bounds.Center + Vector3(-bounds.Extents.x, -bounds.Extents.y, -bounds.Extents.z);
        auto max_pos = bounds.Center + Vector3(bounds.Extents.x, bounds.Extents.y, bounds.Extents.z);
        min_pos = Vector3::Transform(min_pos, world_matrix);
        max_pos = Vector3::Transform(max_pos, world_matrix);
        DirectX::BoundingBox::CreateFromPoints(bounds, min_pos, max_pos);
        if (frustum.Intersects(bounds))
        {
            visible_objects.emplace_back(renderer);
        }
    }
    return visible_objects;
}

int Camera::Order()
{
    return Main() == shared_from_base<Camera>() ? INT_MAX - 20000 : INT_MAX - 30000;
}

void Camera::OnAwake()
{
    if (!m_main_camera_.lock())
    {
        m_main_camera_ = shared_from_base<Camera>();
    }
}

void Camera::OnConstructed()
{
    for (auto &view_proj_matrix_buffer : m_view_proj_matrix_buffers_)
    {
        view_proj_matrix_buffer = std::make_shared<ConstantBuffer>(sizeof(ViewProjection));
        view_proj_matrix_buffer->CreateBuffer();
    }
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

    Gui::PropertyField("RenderTexture", render_texture);

    float color_buf[4];
    EngineUtil::ToFloat4(color_buf, m_background_color_);
    if (ImGui::ColorPicker4("Background Color", color_buf))
    {
        m_background_color_ = Color{color_buf[0], color_buf[1], color_buf[2], color_buf[3]};
        g_RenderEngine->SetBackGroundColor(m_background_color_);
    }
    ImGui::Text("DrawCall Count:%d", m_drawcall_count_);
}

void Camera::OnDraw()
{
    if (BeginRender())
    {
        SetViewProjMatrix();
        m_current_camera_ = shared_from_base<Camera>();
        m_drawcall_count_ = 0;
        auto objects_in_view = FilterVisibleObjects(Renderer::renderers);
        for (auto object : objects_in_view)
        {
            m_drawcall_count_++;
            object->OnDraw();
        }
        Gizmos::Render();
        EndRender();
    }
}

void Camera::OnEnabled()
{
    UpdateManager::SubscribeDrawCall(shared_from_base<Camera>());
}

void Camera::OnDisabled()
{
    UpdateManager::UnsubscribeDrawCall(shared_from_base<Camera>());
}

std::shared_ptr<Camera> Camera::Main()
{
    return m_main_camera_.lock();
}

std::weak_ptr<Camera> Camera::Current()
{
    return m_current_camera_;
}

Matrix Camera::GetViewMatrix() const
{
    const auto transform = GameObject()->Transform();
    return DirectX::XMMatrixLookAtRH(transform->Position(), transform->Position() + transform->Forward(),
                                     transform->Up());
}

Matrix Camera::GetProjectionMatrix() const
{
    const float aspect = static_cast<float>(Application::WindowWidth()) / static_cast<float>(
                             Application::WindowHeight());
    return DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(m_field_of_view_), aspect, 0.3f, 1000.0f);
}
}

CEREAL_REGISTER_TYPE(engine::Camera)