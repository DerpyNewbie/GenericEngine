#include "pch.h"
#include "compute_shader_test_component.h"
#include "Rendering/gpu_resource_manager.h"

namespace engine
{
void ComputeShaderTestComponent::Execute() const
{
    if (m_compute_shader_ == nullptr)
        return;

    m_compute_shader_->Dispatch(1920, 1080, 1, m_material_block_);
}

void ComputeShaderTestComponent::OnConstructed()
{
    m_material_block_ = std::make_shared<MaterialBlock>();
}

void ComputeShaderTestComponent::OnInspectorGui()
{
    if (Gui::PropertyField("ComputeShader", m_compute_shader_))
        m_material_block_->LoadShaderParameters(m_compute_shader_->parameters);

    m_material_block_->OnInspectorGui();

    if (ImGui::Button("Execute"))
    {
        Execute();
    }
}
}