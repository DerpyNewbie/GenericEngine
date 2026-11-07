#include "pch.h"
#include "lighting.h"

namespace engine
{
Lighting *Lighting::Instance()
{
    static auto instance = new Lighting;
    return instance;
}

uint32_t Lighting::GetLightCount() const
{
    return static_cast<uint32_t>(m_lights_.size());
}

void Lighting::AddLight(std::shared_ptr<Light> light)
{
    if (std::ranges::find(m_lights_, light) == m_lights_.end())
        m_lights_.emplace_back(std::move(light));
}

void Lighting::RemoveLight(const std::shared_ptr<Light> &light)
{
    std::erase(m_lights_, light);
}

void Lighting::SetCascadeSlices(
    const std::array<float, RenderingConstants::kShadowCascadeCount> &shadow_cascade_slices)
{
    m_cascade_slices_ = shadow_cascade_slices;
}
}
