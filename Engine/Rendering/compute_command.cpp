#include "pch.h"
#include "compute_command.h"

#include "gpu_resource_manager.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/RootSignature.h"

namespace engine
{
bool ComputeCommand::SetDescriptorTable(const std::shared_ptr<MaterialBlock> &material_block)
{
    const auto resource_group = GpuResourceManager::GetBuffersForMaterial(material_block);
    const auto cmd_list = RenderEngine::CommandList();

    if (!resource_group->UpdateBuffer(material_block))
        return false;
    if (!resource_group->SetBufferToDescriptorTable())
        return false;

    for (int param_i = 0; param_i < kGpuBufferType_Count; ++param_i)
    {
        const auto param_type = static_cast<kGpuUploadType>(param_i);

        if (resource_group->Empty(param_type))
        {
            continue;
        }

        const int root_param_idx = param_i +
                                   RootSignature::kPreDefinedVariableCount;
        const auto itr = resource_group->Begin(param_type);
        const auto desc_handle = itr.handle->handle_gpu;
        cmd_list->SetComputeRootDescriptorTable(root_param_idx, desc_handle);
    }
    return true;
}
ComputeCommand::ComputeCommand(const AssetPtr<ComputeShader> &compute_shader, const std::shared_ptr<MaterialBlock> &material_block, const uint32_t group_count_x, const uint32_t group_count_y, const uint32_t group_count_z) : m_compute_shader_(compute_shader), m_material_block_(material_block), m_group_count_x_(group_count_x), m_group_count_y_(group_count_y),
    m_group_count_z_(group_count_z)
{}

void ComputeCommand::Execute() const
{
    if (m_compute_shader_ == nullptr || m_material_block_ == nullptr)
        return;

    const auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetComputeRootSignature(RootSignature::Get());
    PSOManager::SetComputePipelineState(cmd_list, m_compute_shader_.CastedLock().get());

    SetDescriptorTable(m_material_block_);

    cmd_list->Dispatch(m_group_count_x_, m_group_count_y_, m_group_count_z_);
}
}