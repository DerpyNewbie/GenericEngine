#include "pch.h"
#include "gpu_resource_manager.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"


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

    for (const auto &data : material_block->material_data)
    {
        switch (data->BufferType())
        {
            case kBufferType_ConstantBuffer: {
                auto cb = Object::Instantiate<ConstantBuffer>();
                cb->SetBufferSize(data->SizeInBytes());
                new_group->Insert(AssetPtr<ConstantBuffer>::FromManaged(cb), data);
                break;
            }
            case kBufferType_StructuredBuffer: {
                auto sb = Object::Instantiate<StructuredBuffer>();
                sb->SetBufferSizeAndCount(data->SizeInBytes(), data->Count());
                new_group->Insert(AssetPtr<StructuredBuffer>::FromManaged(sb), data);
            }
            case kBufferType_Texture2D: {
                auto texture_id = data->Data();
                auto texture = Texture2D::GetTexture(*static_cast<TextureId *>(texture_id));
                new_group->Insert(texture, data);
                break;
            }
        }
    }

    m_material_block_buffer_map_.insert({material_block, new_group});
    return new_group;
}
}