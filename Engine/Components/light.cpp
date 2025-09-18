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
std::vector<std::shared_ptr<Light>> Light::m_lights_;
std::shared_ptr<StructuredBuffer> Light::m_lights_buffer_;
std::shared_ptr<DescriptorHandle> Light::m_lights_buffer_handle_;

void Light::UpdateLightCountBuffer()
{
    std::array<LightCountBuffer, 1> light_count_buffer;
    light_count_buffer[0].count = m_lights_.size();

    m_light_count_buffer_->UpdateBuffer(light_count_buffer.data());
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

    for (const auto light : m_lights_)
    {
        light->SetMatrix();
    }

    std::array<LightData, kMaxLightCount> properties;
    for (int i = 0; i < m_lights_.size(); ++i)
        properties[i] = m_lights_[i]->m_light_data_;
    m_lights_buffer_->UpdateBuffer(properties.data());
}

void Light::SetLightCountBuffer()
{
    if (m_light_count_buffer_ == nullptr)
    {
        m_light_count_buffer_ = std::make_shared<ConstantBuffer>(sizeof(int));
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

void Light::SetMatrix()
{
    m_light_data_.view_proj = m_camera_->ProjectionMatrix() * m_camera_->ViewMatrix();
}

void Light::OnInspectorGui()
{
    Gui::PropertyField("Intensity", m_light_data_.intensity);
    Gui::ColorField("LightColor", m_light_data_.color);
}
void Light::OnEnabled()
{
    RenderPipeline::Instance()->AddLight(shared_from_base<Light>());
}

void Light::OnUpdate()
{
    m_light_data_.view_proj = m_camera_->ProjectionMatrix() * m_camera_->ViewMatrix();
}
}