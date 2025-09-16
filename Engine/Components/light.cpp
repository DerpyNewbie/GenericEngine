#include "pch.h"
#include "light.h"
#include "directional_light.h"
#include "gui.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
int Light::m_last_light_count_;
std::shared_ptr<ConstantBuffer> Light::m_light_count_buffer_;
std::vector<std::shared_ptr<Light>> Light::m_lights_;
std::shared_ptr<StructuredBuffer> Light::m_lights_buffer_;

void Light::UpdateLightCountBuffer()
{
    if (m_lights_.empty())
        return;
    int light_count = m_lights_.size();

    m_light_count_buffer_->UpdateBuffer(&light_count);
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
    }

    std::vector<LightData> properties;
    for (const auto light : m_lights_)
        properties.emplace_back(light->m_light_data_);
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
    cmd_list->SetGraphicsRootShaderResourceView(kLightSRV, m_lights_buffer_->GetAddress());
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

void Light::OnUpdate()
{
    m_light_data_.view_proj = m_camera_->ProjectionMatrix() * m_camera_->ViewMatrix();
}
}