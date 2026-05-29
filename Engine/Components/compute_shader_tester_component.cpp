#include "pch.h"
#include "compute_shader_tester_component.h"

#include "Rendering/gpu_resource_manager.h"

namespace engine
{
void ComputeShaderTesterComponent::CreateBuffer()
{
    std::array<ParticleData, 128> particles_data_;
    for (auto i = 0; i < particles_data_.size(); ++i)
    {
        particles_data_[i].pos = Vector2(static_cast<float>(i) / 128, static_cast<float>(i) / 128);
        particles_data_[i].velocity = Vector2(1, 0.5f);
    }

    if (m_structured_buffer_ == nullptr)
    {
        m_structured_buffer_ = std::make_shared<StructuredBuffer>(sizeof(ParticleData), particles_data_.size());
        m_structured_buffer_->CreateBuffer();
    }
    m_structured_buffer_->UpdateBuffer(particles_data_.data());
}

void ComputeShaderTesterComponent::OnInspectorGui()
{
    if (Gui::PropertyField("ComputeShader", m_compute_shader_))
    {
        if (m_compute_shader_ != nullptr)
        {
            CreateBuffer();
            GpuResourceManager::SetGlobalBuffer("ParticleBuffer", m_structured_buffer_);
        }
    }
}

void ComputeShaderTesterComponent::OnUpdate()
{
    m_compute_shader_->Execute();
}
}

CEREAL_REGISTER_TYPE(engine::ComputeShaderTesterComponent)