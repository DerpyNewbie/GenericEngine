#include "pch.h"

#include "gui.h"
#include "application.h"
#include "Components/camera_component.h"

#include "Rendering/render_pipeline.h"

namespace engine
{

void CameraComponent::OnAwake()
{
    m_camera_ = std::make_shared<Camera>();
    m_camera_->SetTransform(GameObject()->Transform());
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

void CameraComponent::OnEnabled()
{
    RenderPipeline::AddCamera(m_camera_);
}

void CameraComponent::OnDisabled()
{
    RenderPipeline::RemoveCamera(m_camera_);
}
}

CEREAL_REGISTER_TYPE(engine::CameraComponent)