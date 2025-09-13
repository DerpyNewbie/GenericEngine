#include "pch.h"
#include "light.h"
#include "directional_light.h"
#include "gui.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"

namespace engine
{
std::shared_ptr<ConstantBuffer> Light::m_light_count_buffer_;

void Light::CreateLightCountBuffer()
{
    m_light_count_buffer_ = std::make_shared<ConstantBuffer>(sizeof(int) * kLightKindCount);
    m_light_count_buffer_->CreateBuffer();
}

void Light::UpdateLightCountBuffer()
{
    int light_count[kLightKindCount];
    light_count[kDirectional] = DirectionalLight::LightCount();

    m_light_count_buffer_->UpdateBuffer(light_count);
}

void Light::SetLightCountBuffer()
{
    if (m_light_count_buffer_ == nullptr)
        return;
    UpdateLightCountBuffer();
    auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetGraphicsRootConstantBufferView(kLightCountCBV, m_light_count_buffer_->GetAddress());
}

void Light::SetLightBuffers()
{
    if (m_light_count_buffer_ == nullptr)
    {
        CreateLightCountBuffer();
    }

    SetLightCountBuffer();
    DirectionalLight::SetDescHandle();
}

void Light::OnInspectorGui()
{
    float intensity = GetIntencity();
    if (Gui::PropertyField("Intensity", intensity))
    {
        SetIntensity(intensity);
    }

    Color color = GetColor();
    if (Gui::ColorField("LightColor", color))
    {
        SetColor(color);
    }
}
}