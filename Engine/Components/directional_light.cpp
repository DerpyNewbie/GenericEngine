#include "pch.h"
#include "directional_light.h"

#include "game_object.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"
#include "Rendering/CabotEngine/Graphics/RootSignature.h"
#include "Rendering/CabotEngine/Graphics/StructuredBuffer.h"

namespace engine
{
std::vector<std::shared_ptr<DirectionalLight>> DirectionalLight::m_directional_lights_;
std::shared_ptr<StructuredBuffer> DirectionalLight::m_directional_lights_buffer_;
int DirectionalLight::m_last_directional_light_count_ = 0;

void DirectionalLight::SetDescHandle()
{
    UpdateBuffer();
    if (m_directional_lights_buffer_ == nullptr)
        return;
    auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetGraphicsRootShaderResourceView(kDirectionalLightSRV, m_directional_lights_buffer_->GetAddress());
}

int DirectionalLight::LightCount()
{
    return m_last_directional_light_count_;
}

void DirectionalLight::UpdateBuffer()
{
    if (m_directional_lights_.empty())
        return;

    if (m_last_directional_light_count_ != m_directional_lights_.size())
    {
        m_last_directional_light_count_ = m_directional_lights_.size();
        m_directional_lights_buffer_ = std::make_shared<StructuredBuffer>(sizeof(DirectionalLightProperty),
                                                                          m_directional_lights_.size());
        m_directional_lights_buffer_->CreateBuffer();
    }
    std::vector<DirectionalLightProperty> properties;
    for (auto directional_light : m_directional_lights_)
        properties.emplace_back(directional_light->m_property_);
    m_directional_lights_buffer_->UpdateBuffer(properties.data());
}

float DirectionalLight::GetIntencity()
{
    return m_property_.m_intensity_;
}

void DirectionalLight::SetIntensity(const float intensity)
{
    m_property_.m_intensity_ = intensity;
}

Color DirectionalLight::GetColor()
{
    return m_property_.color;
}

void DirectionalLight::SetColor(const Color color)
{
    m_property_.color = color;
}

void DirectionalLight::OnAwake()
{
    m_directional_lights_.emplace_back(shared_from_base<DirectionalLight>());
}

void DirectionalLight::OnUpdate()
{
    m_property_.direction = GameObject()->Transform()->Forward();
}
}