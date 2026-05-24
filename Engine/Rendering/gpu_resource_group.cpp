#include "pch.h"
#include "gpu_resource_group.h"

#include "gpu_resource_manager.h"

namespace engine
{
void GpuResourceGroup::UpdateConstantBuffer(const GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block)
{
    gpu_resource.buffer->UploadBuffer(gpu_resource.handle);

    auto cb_data = material_block->GetConstantBufferData(gpu_resource.name);
    if (cb_data->is_dirty)
        gpu_resource.buffer->UpdateBuffer(cb_data->Data());
}

void GpuResourceGroup::UpdateStructuredBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block)
{
    auto sb_data = material_block->GetStructuredBufferData(gpu_resource.name);
    if (sb_data->is_size_changed)
    {
        sb_data->is_size_changed = false;
        gpu_resource.buffer = std::make_shared<StructuredBuffer>(sb_data->Stride(), sb_data->Count());
        gpu_resource.buffer->UploadBuffer(gpu_resource.handle);
    }
    if (sb_data->is_dirty)
        gpu_resource.buffer->UpdateBuffer(sb_data->Data());
}

void GpuResourceGroup::UpdateTextureBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block)
{
    auto tex_data = material_block->GetTextureBufferData(gpu_resource.name);
    if (tex_data->is_dirty)
    {
        gpu_resource.buffer = TextureCollection::GetTexture(tex_data->Data());
        gpu_resource.buffer->UploadBuffer(gpu_resource.handle);
    }
}

bool GpuResourceGroup::SetGlobalResource(GpuResource &gpu_resource)
{
    auto global_resource = GpuResourceManager::GetGlobalBuffer(gpu_resource.name);
    if (global_resource == nullptr)
    {
        return false;
    }

    gpu_resource.buffer = global_resource;
    gpu_resource.buffer->UploadBuffer(gpu_resource.handle);

    return true;
}

void GpuResourceGroup::Insert(const std::shared_ptr<BufferBase> &buffer, const std::shared_ptr<MaterialDataBase> &material_data, kBufferType buffer_type, bool is_external)
{
    auto data = buffer;

    const auto upload_type = data->BufferType();
    data->CreateBuffer();
    GpuResource gpu_resource;
    gpu_resource.name = material_data->parameter.name;
    gpu_resource.buffer = buffer;
    gpu_resource.buffer_type = buffer_type;

    m_gpu_resources_[upload_type].try_emplace(material_data->parameter.index, gpu_resource);
}

bool GpuResourceGroup::Empty(const kGpuUploadType buffer_type) const
{
    return m_gpu_resources_[buffer_type].empty();
}

GpuResource GpuResourceGroup::Begin(const kGpuUploadType buffer_type)
{
    return m_gpu_resources_[buffer_type].begin()->second;
}

GpuResource GpuResourceGroup::End(const kGpuUploadType buffer_type)
{
    return m_gpu_resources_[buffer_type].begin()->second;
}

bool GpuResourceGroup::UpdateBuffer(const std::shared_ptr<MaterialBlock> &material_block) const
{
    for (auto gpu_resources : m_gpu_resources_)
    {
        for (auto gpu_resource : gpu_resources | std::views::values)
        {
            if (gpu_resource.handle == nullptr)
                return false;

            if (SetGlobalResource(gpu_resource))
                continue;

            if (gpu_resource.buffer == nullptr)
                return false;

            switch (gpu_resource.buffer_type)
            {
                case kBufferType_ConstantBuffer: {
                    UpdateConstantBuffer(gpu_resource, material_block);
                    break;
                }
                case kBufferType_StructuredBuffer: {
                    UpdateStructuredBuffer(gpu_resource, material_block);
                    break;
                }
                case kBufferType_Texture2D: {
                    UpdateTextureBuffer(gpu_resource, material_block);
                    break;
                }
            }
        }
    }

    return true;
}

bool GpuResourceGroup::SetBufferToDescriptorTable()
{
    if (!m_is_dirty_)
        return true;

    for (auto &gpu_resources : m_gpu_resources_)
    {
        auto itr = 0;
        const auto handles = DescriptorHeap::AllocateLinedUp(gpu_resources.size());
        for (auto &gpu_resource : gpu_resources | std::views::values)
        {
            if (gpu_resource.buffer == nullptr)
                return false;

            const auto handle = handles[itr];
            gpu_resource.buffer->UploadBuffer(handle);
            gpu_resource.handle = handle;

            ++itr;
        }
    }

    m_is_dirty_ = false;

    return true;
}
}