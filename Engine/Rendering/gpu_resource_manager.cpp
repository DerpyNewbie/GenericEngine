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

    for (const auto &data : material_block->m_constant_buffer_data_)
    {
        auto cb = std::make_shared<>();
    }
    
    for (const auto &data : material_block->material_data)
    {
        switch (data->BufferType())
        {
            case kBufferType_ConstantBuffer: {
                auto cb = std::make_shared<ConstantBuffer>(data->SizeInBytes());
                new_group->Insert(cb, data);
                break;
            }
            case kBufferType_StructuredBuffer: {
                auto sb = std::make_shared<StructuredBuffer>(data->SizeInBytes(), data->Count());
                new_group->Insert(sb, data);
            }
            case kBufferType_Texture2D: {
                auto texture = data->Data();
                auto texture_buffer = TextureCollection::GetTexture(*static_cast<AssetPtr<Texture2D> *>(texture));
                new_group->Insert(texture_buffer, data);
                break;
            }
        }
    }

    m_material_block_buffer_map_.insert({material_block, new_group});
    return new_group;
}
}