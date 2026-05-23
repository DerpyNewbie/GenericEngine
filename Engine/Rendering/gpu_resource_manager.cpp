#include "pch.h"
#include "gpu_resource_manager.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"
#include "texture_collection.h"

namespace engine
{
std::shared_ptr<GpuResourceGroup> GpuResourceManager::GetBuffersForMaterial(std::shared_ptr<MaterialBlock> material_block)
{
    const auto it = m_material_block_buffer_map_.find(material_block);
    if (it != m_material_block_buffer_map_.end())
    {
        return it->second;
    }

    auto new_group = std::make_shared<GpuResourceGroup>();

    for (const auto &data : material_block->m_constant_buffer_data_ | std::views::values)
    {
        auto cb = std::make_shared<ConstantBuffer>(data->Size());

        new_group->Insert(cb, data, kBufferType_ConstantBuffer);
    }
    for (const auto &data : material_block->m_structured_buffer_data_ | std::views::values)
    {
        auto sb = std::make_shared<StructuredBuffer>(data->Stride(), data->Count());

        new_group->Insert(sb, data, kBufferType_StructuredBuffer);
    }
    for (const auto &data : material_block->m_texture_buffer_data_ | std::views::values)
    {
        auto texture = data->Data();

        auto texture_buffer = TextureCollection::GetTexture(texture);

        new_group->Insert(texture_buffer, data, kBufferType_Texture2D);
    }

    m_material_block_buffer_map_.insert({material_block, new_group});
    return new_group;
}
}