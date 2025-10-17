#include "pch.h"
#include "light.h"
#include "gui.h"
#include "Rendering/lighting.h"
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
    if (m_light_count_buffer_ == nullptr)
    {
        m_light_count_buffer_ = std::make_shared<ConstantBuffer>(sizeof(LightCountBuffer));
        m_light_count_buffer_->CreateBuffer();
    }

    LightCountBuffer lcb(static_cast<uint32_t>(Lighting::Instance()->m_lights_.size()));

    m_light_count_buffer_->UpdateBuffer(&lcb);
}

void Light::UpdateLightBuffer()
{
    if (Lighting::Instance()->m_lights_.empty())
        return;

    if (m_lights_buffer_ == nullptr)
    {
        m_lights_buffer_ = std::make_shared<StructuredBuffer>(sizeof(LightData),
                                                              RenderingConstants::kMaxLightCount);
        m_lights_buffer_->CreateBuffer();
        m_lights_buffer_handle_ = m_lights_buffer_->UploadBuffer();
    }

    std::array<LightData, RenderingConstants::kMaxLightCount> properties;
    for (int i = 0; i < Lighting::Instance()->m_lights_.size(); ++i)
        properties[i] = Lighting::Instance()->m_lights_[i]->m_light_data_;
    m_lights_buffer_->UpdateBuffer(properties.data());
}

void Light::SetLightCountBuffer()
{
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

    if (Gui::BoolField("CastShadow", m_has_shadow_))
    {
        if (m_has_shadow_ == false)
        {
            m_light_data_.cast_shadow = false;
            Lighting::Instance()->RemoveShadow(shared_from_base<Light>());
        }
        else
            Lighting::Instance()->TryApplyShadow(shared_from_base<Light>());
    }
}

void Light::OnEnabled()
{

    Lighting::Instance()->AddLight(shared_from_base<Light>());
    if (m_has_shadow_)
        Lighting::Instance()->TryApplyShadow(shared_from_base<Light>());
}

void Light::OnDisabled()
{
    Lighting::Instance()->RemoveLight(shared_from_base<Light>());
}

void Light::OnDestroy()
{
    Lighting::Instance()->RemoveLight(shared_from_base<Light>());
}
}

CEREAL_REGISTER_TYPE(engine::Light)