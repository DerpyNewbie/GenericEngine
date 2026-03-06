#include "pch.h"
#include "raytracing_component.h"

#include "update_manager.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"
#include "Rendering/raytracing/raytracing_manager.h"

namespace engine
{
void RaytracingComponent::OnInspectorGui()
{
    Gui::PropertyField("Target Camera", m_target_camera_);
    Gui::PropertyField("Output Texture", m_output_texture_);
    if (Gui::PropertyField("Raytracing Shader", m_raytracing_shader_))
    {
        m_shader_table_ = std::make_shared<ShaderTable>(m_raytracing_shader_.CastedLock());
    }
}

void RaytracingComponent::OnEnabled()
{
    UpdateManager::SubscribeRender(shared_from_base<RaytracingComponent>());
}

void RaytracingComponent::OnDisabled()
{
    UpdateManager::UnsubscribeRender(shared_from_base<RaytracingComponent>());
}

void RaytracingComponent::Render()
{
    RaytracingManager::RequestRaytracing(m_target_camera_.CastedLock(), m_raytracing_shader_.CastedLock(), m_shader_table_, m_output_texture_.CastedLock());
}

std::shared_ptr<Texture2D> RaytracingComponent::GetResult()
{
    return m_output_texture_.CastedLock();
}

AssetPtr<CameraComponent> RaytracingComponent::TargetCamera()
{
    return m_target_camera_;
}

void RaytracingComponent::SetTexture(const AssetPtr<UavTexture> &uav_texture)
{
    m_output_texture_ = uav_texture;
}

void RaytracingComponent::SetTargetCamera(const AssetPtr<CameraComponent> &target_camera)
{
    m_target_camera_ = target_camera;
}

void RaytracingComponent::SetRaytracingShader(const AssetPtr<RaytracingShader> &raytracing_shader)
{
    m_raytracing_shader_ = raytracing_shader;
}
}

CEREAL_REGISTER_TYPE(engine::RaytracingComponent)