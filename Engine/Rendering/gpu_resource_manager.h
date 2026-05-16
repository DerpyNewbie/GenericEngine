#pragma once
#include "gpu_resource_group.h"
#include "material.h"

namespace engine
{
class GpuResourceManager
{
    inline static std::map<std::shared_ptr<MaterialBlock>, std::shared_ptr<GpuResourceGroup>> m_material_block_buffer_map_;

public:
    static std::shared_ptr<GpuResourceGroup> GetBuffersForMaterial(std::shared_ptr<MaterialBlock> material_block);

};
}