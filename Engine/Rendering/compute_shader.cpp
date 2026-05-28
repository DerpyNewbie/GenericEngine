#include "pch.h"
#include "compute_shader.h"

#include "gpu_resource_manager.h"
#include "CabotEngine/Graphics/PSOManager.h"
#include "CabotEngine/Graphics/RenderEngine.h"
#include "CabotEngine/Graphics/RootSignature.h"

namespace engine
{
bool ComputeShader::SetDescriptorTable(const std::shared_ptr<MaterialBlock> &material_block)
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

void ComputeShader::OnInspectorGui()
{}

void ComputeShader::Dispatch(const uint32_t group_count_x, const uint32_t group_count_y, const uint32_t group_count_z, const std::shared_ptr<MaterialBlock> &material_block) const
{
    const auto cmd_list = RenderEngine::CommandList();
    cmd_list->SetComputeRootSignature(RootSignature::Get());
    PSOManager::SetComputePipelineState(cmd_list, this);

    SetDescriptorTable(material_block);

    cmd_list->Dispatch(group_count_x, group_count_y, group_count_z);
}
}