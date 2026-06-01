#include "pch.h"
#include "compute_shader_component.h"
#include "Rendering/gpu_resource_manager.h"
#include "Rendering/render_pipeline.h"

namespace engine
{
void ComputeShaderComponent::Execute() const
{
    if (m_compute_shader_.CastedLock() == nullptr)
        return;

    RenderPipeline::Submit(m_compute_shader_, m_material_block_, m_group_count_x_, m_group_count_y_, m_group_count_z_);
}

void ComputeShaderComponent::CreateMaterialBlock()
{
    if (m_compute_shader_.CastedLock() == nullptr)
        return;

    m_material_block_ = Instantiate<MaterialBlock>("Material Block of " + Name());
    m_material_block_->LoadShaderParameters(m_compute_shader_->parameters);
}

void ComputeShaderComponent::OnConstructed()
{
    m_material_block_ = std::make_shared<MaterialBlock>();
}

void ComputeShaderComponent::OnInspectorGui()
{
    int group_count_x = static_cast<int>(m_group_count_x_);
    int group_count_y = static_cast<int>(m_group_count_y_);
    int group_count_z = static_cast<int>(m_group_count_z_);

    if (Gui::PropertyField("GroupCount X", group_count_x))
        m_group_count_x_ = group_count_x;
    if (Gui::PropertyField("GroupCount Y", group_count_y))
        m_group_count_y_ = group_count_y;
    if (Gui::PropertyField("GroupCount Z", group_count_z))
        m_group_count_z_ = group_count_z;
    
    if (Gui::PropertyField("ComputeShader", m_compute_shader_))
    {
        m_material_block_ = std::make_shared<MaterialBlock>();
        m_material_block_->LoadShaderParameters(m_compute_shader_->parameters);
    }

    m_material_block_->OnInspectorGui();

    if (ImGui::Button("Reconstruct Material Block"))
    {
        CreateMaterialBlock();
    }

    if (m_compute_shader_.CastedLock() == nullptr)
        return;

    ImGui::Checkbox("Enable Update", &m_enable_update_);
    
    if (ImGui::Button("Execute"))
    {
        Execute();
    }
}
void ComputeShaderComponent::OnUpdate()
{
    if (m_enable_update_)
        Execute();
}

void ComputeShaderComponent::SetGroupCount(const uint32_t group_count_x, const uint32_t group_count_y, const uint32_t group_count_z)
{
    m_group_count_x_ = group_count_x;
    m_group_count_y_ = group_count_y;
    m_group_count_z_ = group_count_z;
}

void ComputeShaderComponent::SetBuffer(const std::string &name, const std::shared_ptr<BufferBase> &buffer) const
{
    const auto gpu_resource_group = GpuResourceManager::GetBuffersForMaterial(m_material_block_);
    gpu_resource_group->SetBuffer(name, buffer);
}

std::shared_ptr<BufferBase> ComputeShaderComponent::GetBuffer(const std::string &name) const
{
    const auto gpu_resource_group = GpuResourceManager::GetBuffersForMaterial(m_material_block_);

    return gpu_resource_group->GetBuffer(name);
}
}

CEREAL_REGISTER_TYPE(engine::ComputeShaderComponent)