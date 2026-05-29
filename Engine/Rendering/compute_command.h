#pragma once
#include "compute_shader.h"
#include "Asset/asset_ptr.h"

namespace engine
{
class ComputeCommand
{
    AssetPtr<ComputeShader> m_compute_shader_;
    std::shared_ptr<MaterialBlock> m_material_block_;
    uint32_t m_group_count_x_;
    uint32_t m_group_count_y_;
    uint32_t m_group_count_z_;

    static bool SetDescriptorTable(const std::shared_ptr<MaterialBlock> &material_block);

public:
    ComputeCommand(const AssetPtr<ComputeShader> &compute_shader, const std::shared_ptr<MaterialBlock> &material_block, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);

    void Execute() const;
};
}