#include "pch.h"
#include "light.h"
#include "gui.h"
#include "Rendering/render_pipeline.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
std::shared_ptr<ConstantBuffer> Light::m_light_count_buffer_;
std::shared_ptr<StructuredBuffer> Light::m_lights_buffer_;
std::shared_ptr<DescriptorHandle> Light::m_lights_buffer_handle_;

void Light::UpdateLightCountBuffer()
{
    LightCountBuffer lcb(RenderPipeline::Instance()->m_lights_.size());

    m_light_count_buffer_->UpdateBuffer(&lcb);
}

void Light::UpdateLightBuffer()
{
    if (RenderPipeline::Instance()->m_lights_.empty())
        return;

    if (m_lights_buffer_ == nullptr)
    {
        m_lights_buffer_ = std::make_shared<StructuredBuffer>(sizeof(LightData),
                                                              RenderPipeline::kMaxLightCount);
        m_lights_buffer_->CreateBuffer();
        m_lights_buffer_handle_ = m_lights_buffer_->UploadBuffer();
    }

    std::array<LightData, RenderPipeline::kMaxLightCount> properties;
    for (int i = 0; i < RenderPipeline::Instance()->m_lights_.size(); ++i)
        properties[i] = RenderPipeline::Instance()->m_lights_[i]->m_light_data_;
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

void Light::OnEnabled()
{
    RenderPipeline::Instance()->AddLight(shared_from_base<Light>());
}

void Light::OnDisabled()
{
    RenderPipeline::Instance()->RemoveLight(shared_from_base<Light>());
}

void Light::OnDestroy()
{
    RenderPipeline::Instance()->RemoveLight(shared_from_base<Light>());
}
}

CEREAL_REGISTER_TYPE(engine::Light)