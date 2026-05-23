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

std::shared_ptr<BufferBase> GpuResourceManager::GetGlobalBuffer(const std::string &name)
{
    const auto it = m_global_resources_.find(name);
    if (it == m_global_resources_.end())
        return nullptr;

    return it->second;
}

void GpuResourceManager::SetGlobalBuffer(const std::string &name, const std::shared_ptr<ConstantBuffer> &buffer)
{
    m_global_resources_[name] = buffer;
}

void GpuResourceManager::SetGlobalBuffer(const std::string &name, const std::shared_ptr<StructuredBuffer> &buffer)
{
    m_global_resources_[name] = buffer;
}

void GpuResourceManager::SetGlobalTexture(const std::string &name, const std::shared_ptr<Texture2D> &texture)
{
    const auto texture_buffer = TextureCollection::GetTexture(texture);
    m_global_resources_[name] = texture_buffer;
}

void GpuResourceManager::SetGlobalInt(const std::string &name, const int data)
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

void GpuResourceManager::SetGlobalFloat(const std::string &name, const float data)
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

void GpuResourceManager::SetGlobalFloatArray(const std::string &name, const std::vector<float> &data)
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

void GpuResourceManager::SetGlobalVector(const std::string &name, const Vector3 &data)
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

void GpuResourceManager::SetGlobalVectorArray(const std::string &name, const std::vector<Vector3> &data)
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

void GpuResourceManager::SetGlobalMatrix(const std::string &name, const Matrix &data)
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

void GpuResourceManager::SetGlobalMatrixArray(const std::string &name, const std::vector<Matrix> &data)
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

void GpuResourceManager::SetGlobalColor(const std::string &name, const Color &data)
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