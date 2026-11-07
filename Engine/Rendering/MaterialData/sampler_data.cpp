#include "pch.h"
#include "sampler_data.h"

namespace engine
{
SamplerData::SamplerData(const ShaderParameter &shader_param) : BufferDataBase(shader_param)
{}

void SamplerData::OnInspectorGui()
{
    ImGui::PushID(this);
    ImGui::Text("%s", parameter.name.c_str());

    int filter = m_filter_;
    int address = m_address_;
    bool changed = ImGui::Combo("Filter", &filter, "Point\0Linear\0Anisotropic\0\0");
    changed |= ImGui::Combo("Address", &address, "Wrap\0Clamp\0Mirror\0\0");
    if (changed)
        SetState(filter, address);

    ImGui::PopID();
}

void SamplerData::SetState(const int filter, const int address)
{
    m_filter_ = std::clamp(filter, 0, 2);
    m_address_ = std::clamp(address, 0, 2);
    is_dirty = true;
}

int SamplerData::Filter() const
{
    return m_filter_;
}

int SamplerData::Address() const
{
    return m_address_;
}

kBufferType SamplerData::BufferType()
{
    return kBufferType_Sampler;
}
}

CEREAL_REGISTER_TYPE(engine::SamplerData)