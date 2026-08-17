#include "pch.h"
#include "gpu_resource_manager.h"

#include "byte_address_buffer_data.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"
#include "CabotEngine/Graphics/StructuredBuffer.h"
#include "texture_collection.h"
#include "CabotEngine/Graphics/ByteAddressBuffer.h"

namespace engine
{
std::shared_ptr<GpuResourceGroup> GpuResourceManager::GetBuffersForMaterial(
    std::shared_ptr<MaterialBlock> material_block)
{
    const auto it = m_material_block_buffer_map_.find(material_block);
    if (it != m_material_block_buffer_map_.end())
    {
        return it->second;
    }

    auto new_group = std::make_shared<GpuResourceGroup>();

    for (const auto& data : material_block->m_buffer_data_ | std::views::values)
    {
        switch (data->BufferType())
        {
        case kBufferType_ConstantBuffer:
            {
                const auto cb_data = std::reinterpret_pointer_cast<ConstantBufferData>(data);
                auto cb = std::make_shared<ConstantBuffer>(cb_data->Size());

                new_group->Insert(cb, data, kBufferType_ConstantBuffer, kGpuBufferType_CBV);
                break;
            }
        case kBufferType_StructuredBuffer:
            {
                const auto sb_data = std::reinterpret_pointer_cast<StructuredBufferData>(data);
                auto sb = std::make_shared<StructuredBuffer>(sb_data->Stride(), sb_data->Count());

                new_group->Insert(sb, data, kBufferType_StructuredBuffer,
                                  data->parameter.is_unordered_access ? kGpuBufferType_UAV : kGpuBufferType_SRV);
                break;
            }
        case kBufferType_Texture2D:
            {
                const auto tex_data = std::reinterpret_pointer_cast<TextureBufferData>(data);
                auto texture = tex_data->Data();
                auto texture_buffer = texture == nullptr ? nullptr : TextureCollection::GetTexture(texture);

                new_group->Insert(texture_buffer, data, kBufferType_Texture2D, kGpuBufferType_SRV);
                break;
            }
        case kBufferType_UavTexture:
            {
                const auto uav_tex_data = std::reinterpret_pointer_cast<UavTextureBufferData>(data);
                auto texture = uav_tex_data->Data();
                auto texture_buffer = texture == nullptr ? nullptr : TextureCollection::GetRenderTexture(texture);

                new_group->Insert(texture_buffer, data, kBufferType_UavTexture, kGpuBufferType_UAV);
                break;
            }
        case kBufferType_ByteAddressBuffer:
            {
                const auto byte_address_data = std::reinterpret_pointer_cast<ByteAddressBufferData>(data);
                auto byte_address_buffer = std::make_shared<ByteAddressBuffer>(byte_address_data->Count());

                new_group->Insert(byte_address_buffer, data, kBufferType_ByteAddressBuffer,
                                  data->parameter.is_unordered_access ? kGpuBufferType_UAV : kGpuBufferType_SRV);
            }
        }
    }

    m_material_block_buffer_map_.insert({material_block, new_group});
    return new_group;
}

std::shared_ptr<BufferBase> GpuResourceManager::GetGlobalBuffer(const std::string& name)
{
    const auto it = m_global_resources_.find(name);
    if (it == m_global_resources_.end())
        return nullptr;

    return it->second;
}

void GpuResourceManager::SetGlobalBufferData(const std::string& name,
                                             const std::shared_ptr<BufferDataBase>& buffer_data)
{
    const auto it = m_global_resources_.find(name);
    if (it == m_global_resources_.end())
    {
        std::shared_ptr<BufferBase> buffer;
        switch (buffer_data->BufferType())
        {
        case kBufferType_ConstantBuffer:
            {
                const auto cb_data = std::reinterpret_pointer_cast<ConstantBufferData>(buffer_data);
                buffer = std::make_shared<ConstantBuffer>(cb_data->Size());
                break;
            }
        case kBufferType_StructuredBuffer:
            {
                const auto sb_data = std::reinterpret_pointer_cast<StructuredBufferData>(buffer_data);
                buffer = std::make_shared<StructuredBuffer>(sb_data->Stride(), sb_data->Count());
                break;
            }
        case kBufferType_Texture2D:
            {
                const auto tex_data = std::reinterpret_pointer_cast<TextureBufferData>(buffer_data);
                buffer = TextureCollection::GetTexture(tex_data->Data());
                break;
            }
        case kBufferType_UavTexture:
            {
                const auto uav_tex_data = std::reinterpret_pointer_cast<UavTextureBufferData>(buffer_data);
                buffer = TextureCollection::GetRenderTexture(uav_tex_data->Data());
                break;
            }
        }

        m_global_resources_[name] = buffer;
    }
    else
    {
        switch (buffer_data->BufferType())
        {
        case kBufferType_ConstantBuffer:
            {
                const auto cb_data = std::reinterpret_pointer_cast<ConstantBufferData>(buffer_data);
                if (it->second->IsValid())
                    it->second->UpdateBuffer(cb_data->Data());
                break;
            }
        case kBufferType_StructuredBuffer:
            {
                const auto sb_data = std::reinterpret_pointer_cast<StructuredBufferData>(buffer_data);
                if (it->second->IsValid())
                it->second->UpdateBuffer(sb_data->Data());
                break;
            }
        case kBufferType_Texture2D:
            {
                const auto tex_data = std::reinterpret_pointer_cast<TextureBufferData>(buffer_data);
                it->second = TextureCollection::GetTexture(tex_data->Data());
                break;
            }
        case kBufferType_UavTexture:
            {
                const auto uav_tex_data = std::reinterpret_pointer_cast<UavTextureBufferData>(buffer_data);
                it->second = TextureCollection::GetRenderTexture(uav_tex_data->Data());
                break;
            }
        }
    }
}

void GpuResourceManager::SetGlobalBuffer(const std::string& name, const std::shared_ptr<ConstantBuffer>& buffer)
{
    m_global_resources_[name] = buffer;
}

void GpuResourceManager::SetGlobalBuffer(const std::string& name, const std::shared_ptr<StructuredBuffer>& buffer)
{
    m_global_resources_[name] = buffer;
}

void GpuResourceManager::SetGlobalBuffer(const std::string& name, const std::shared_ptr<ByteAddressBuffer>& buffer)
{
    m_global_resources_[name] = buffer;
}

void GpuResourceManager::SetGlobalTexture(const std::string& name, const std::shared_ptr<Texture2D>& texture)
{
    const auto texture_buffer = TextureCollection::GetTexture(texture);
    m_global_resources_[name] = texture_buffer;
}

void GpuResourceManager::SetGlobalInt(const std::string& name, const int data)
{
    const auto it = m_global_resources_.find(name);
    if (it != m_global_resources_.end())
    {
        it->second->UpdateBuffer(&data);
        return;
    }

    const auto cb = std::make_shared<ConstantBuffer>(sizeof(int));
    cb->CreateBuffer();
    cb->UpdateBuffer(&data);

    m_global_resources_[name] = cb;
}

void GpuResourceManager::SetGlobalFloat(const std::string& name, const float data)
{
    const auto it = m_global_resources_.find(name);
    if (it != m_global_resources_.end())
    {
        it->second->UpdateBuffer(&data);
        return;
    }

    const auto cb = std::make_shared<ConstantBuffer>(sizeof(float));
    cb->CreateBuffer();
    cb->UpdateBuffer(&data);

    m_global_resources_[name] = cb;
}

void GpuResourceManager::SetGlobalFloatArray(const std::string& name, const std::vector<float>& data)
{
    const auto it = m_global_resources_.find(name);
    if (it != m_global_resources_.end())
    {
        it->second->UpdateBuffer(data.data());
        return;
    }

    const auto sb = std::make_shared<StructuredBuffer>(sizeof(float), data.size());
    sb->CreateBuffer();
    sb->UpdateBuffer(data.data());
    m_global_resources_[name] = sb;
}

void GpuResourceManager::SetGlobalVector(const std::string& name, const Vector3& data)
{
    const auto it = m_global_resources_.find(name);
    if (it != m_global_resources_.end())
    {
        it->second->UpdateBuffer(&data);
        return;
    }

    const auto cb = std::make_shared<ConstantBuffer>(sizeof(Vector3));
    cb->CreateBuffer();
    cb->UpdateBuffer(&data);

    m_global_resources_[name] = cb;
}

void GpuResourceManager::SetGlobalVectorArray(const std::string& name, const std::vector<Vector3>& data)
{
    const auto it = m_global_resources_.find(name);
    if (it != m_global_resources_.end())
    {
        it->second->UpdateBuffer(data.data());
        return;
    }

    const auto sb = std::make_shared<StructuredBuffer>(sizeof(Vector3), data.size());
    sb->CreateBuffer();
    sb->UpdateBuffer(data.data());
    m_global_resources_[name] = sb;
}

void GpuResourceManager::SetGlobalMatrix(const std::string& name, const Matrix& data)
{
    const auto it = m_global_resources_.find(name);
    if (it != m_global_resources_.end())
    {
        it->second->UpdateBuffer(&data);
        return;
    }

    const auto cb = std::make_shared<ConstantBuffer>(sizeof(Matrix));
    cb->CreateBuffer();
    cb->UpdateBuffer(&data);

    m_global_resources_[name] = cb;
}

void GpuResourceManager::SetGlobalMatrixArray(const std::string& name, const std::vector<Matrix>& data)
{
    const auto it = m_global_resources_.find(name);
    if (it != m_global_resources_.end())
    {
        it->second->UpdateBuffer(data.data());
        return;
    }

    const auto sb = std::make_shared<StructuredBuffer>(sizeof(Matrix), data.size());
    sb->CreateBuffer();
    sb->UpdateBuffer(data.data());
    m_global_resources_[name] = sb;
}

void GpuResourceManager::SetGlobalColor(const std::string& name, const Color& data)
{
    const auto it = m_global_resources_.find(name);
    if (it != m_global_resources_.end())
    {
        it->second->UpdateBuffer(&data);
        return;
    }

    const auto cb = std::make_shared<ConstantBuffer>(sizeof(Color));
    cb->CreateBuffer();
    cb->UpdateBuffer(&data);

    m_global_resources_[name] = cb;
}
}
