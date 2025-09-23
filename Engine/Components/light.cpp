#include "pch.h"
#include "light.h"
#include "directional_light.h"
#include "gui.h"
#include "Rendering/render_pipeline.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
int Light::m_last_light_count_;
std::shared_ptr<ConstantBuffer> Light::m_light_count_buffer_;
std::vector<std::weak_ptr<Light>> Light::m_lights_;
std::shared_ptr<StructuredBuffer> Light::m_lights_buffer_;
std::shared_ptr<DescriptorHandle> Light::m_lights_buffer_handle_;

void Light::UpdateLightCountBuffer()
{
    LightCountBuffer lcb(m_lights_.size());

    m_light_count_buffer_->UpdateBuffer(&lcb);
}

void Light::UpdateLightBuffer()
{
    m_last_light_count_ = m_lights_.size();
    if (m_lights_.empty())
        return;

    if (m_lights_buffer_ == nullptr)
    {
        m_lights_buffer_ = std::make_shared<StructuredBuffer>(sizeof(LightData),
                                                              kMaxLightCount);
        m_lights_buffer_->CreateBuffer();
        m_lights_buffer_handle_ = m_lights_buffer_->UploadBuffer();
    }

    std::array<LightData, kMaxLightCount> properties;
    for (int i = 0; i < m_lights_.size(); ++i)
        properties[i] = m_lights_[i].lock()->m_light_data_;
    m_lights_buffer_->UpdateBuffer(properties.data());
}

void Light::SetLightCountBuffer()
{
    if (m_light_count_buffer_ == nullptr)
    {
        m_light_count_buffer_ = std::make_shared<ConstantBuffer>(sizeof(LightCountBuffer));
        m_light_count_buffer_->CreateBuffer();
    }

    UpdateLightCountBuffer();
    const auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetGraphicsRootConstantBufferView(kLightCountCBV, m_light_count_buffer_->GetAddress());
}

void Light::SetLightBuffer()
{
    UpdateLightBuffer();

    if (m_lights_buffer_ == nullptr)
        return;

    const auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetGraphicsRootDescriptorTable(kLightSRV, m_lights_buffer_handle_->HandleGPU);
}

void Light::SetBuffers()
{
    SetLightCountBuffer();
    SetLightBuffer();
}

void Light::OnInspectorGui()
{
    Gui::PropertyField("Intensity", m_light_data_.intensity);
    Gui::ColorField("LightColor", m_light_data_.color);
}

void Light::SetCamera(const std::shared_ptr<CameraComponent> camera)
{
    camera->m_property_.far_plane = 20;
    camera->m_property_.view_mode = kViewMode::kOrthographic;
    m_camera_ = camera;
}

void Light::OnEnabled()
{
    RenderPipeline::Instance()->AddLight(shared_from_base<Light>());
    m_lights_.emplace_back(shared_from_base<Light>());
}
void Light::OnDisabled()
{
    RenderPipeline::Instance()->RemoveLight(shared_from_base<Light>());
    auto this_light = shared_from_base<Light>();
    std::erase_if(m_lights_, [this_light](const std::weak_ptr<Light> &light) {
        return this_light == light.lock();
    });
}

void Light::OnUpdate()
{
    m_light_data_.view = m_camera_->ViewMatrix();
    m_light_data_.proj = m_camera_->ProjectionMatrix();
}
}