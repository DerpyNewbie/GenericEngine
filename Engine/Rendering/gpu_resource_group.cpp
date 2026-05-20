#include "pch.h"
#include "gpu_resource_group.h"

#include "gpu_resource_manager.h"

namespace engine
{
int *ShaderDataIndex::GetLengthField(kGpuUploadType type)
{
    switch (type)
    {
        case kGpuBufferType_CBV:
            return &cbv_length;
        case kGpuBufferType_SRV:
            return &srv_length;
        case kGpuBufferType_UAV:
            return &uav_length;
        default:
            static_assert("Invalid buffer type");
            return nullptr;
    }
}

int ShaderDataIndex::GetLength(const kGpuUploadType type) const
{
    switch (type)
    {
        case kGpuBufferType_CBV:
            return cbv_length;
        case kGpuBufferType_SRV:
            return srv_length;
        case kGpuBufferType_UAV:
            return uav_length;
        default:
            static_assert("Invalid buffer type");
            return 0;
    }
}

int ShaderDataIndex::GetOffset(kGpuUploadType type) const
{
    int offset = 0;

    // fall-through
    switch (type)
    {
        case kGpuBufferType_UAV:
            offset += srv_length;
        case kGpuBufferType_SRV:
            offset += cbv_length;
        case kGpuBufferType_CBV:
        default:
            break;
    }

    return offset;
}

int ShaderDataIndex::GetFullLength() const
{
    return cbv_length + srv_length + uav_length;
}

void GpuResourceGroup::Insert(const std::shared_ptr<BufferBase> &buffer, const std::shared_ptr<IMaterialData> &material_data, bool is_external)
{
    const auto buffer_type = buffer->BufferType();
    buffer->CreateBuffer();
    GpuResource gpu_resource;
    gpu_resource.is_external = is_external;
    gpu_resource.name = material_data->parameter.name;
    gpu_resource.buffer = buffer;
    gpu_resource.buffer_type = material_data->BufferType();
    m_gpu_resources_.insert(End(buffer_type), gpu_resource);

    const auto field = m_shader_index_.GetLengthField(buffer_type);
    ++(*field);
}

bool GpuResourceGroup::Empty(const kGpuUploadType buffer_type) const
{
    return m_shader_index_.GetLength(buffer_type) == 0;
}

std::vector<GpuResource>::iterator GpuResourceGroup::Begin(const kGpuUploadType buffer_type)
{
    const auto buffer_offset = m_shader_index_.GetOffset(buffer_type);
    return m_gpu_resources_.begin() + buffer_offset;
}

std::vector<GpuResource>::iterator GpuResourceGroup::End(const kGpuUploadType buffer_type)
{
    return Begin(buffer_type) + m_shader_index_.GetLength(buffer_type);
}

bool GpuResourceGroup::SetBufferWithName(const std::shared_ptr<BufferBase> &buffer, const std::string &name)
{
    const auto it = std::ranges::find_if(m_gpu_resources_, [&name](const GpuResource &material_data) {
        return material_data.name == name;
    });

    if (it == m_gpu_resources_.end())
        return false;

    it->buffer = buffer;
    return true;
}

bool GpuResourceGroup::UpdateBuffer(const std::shared_ptr<MaterialBlock> &material_block) const
{
    const auto &material_data = material_block->material_data;
    for (auto gpu_resource : m_gpu_resources_)
    {
        if (gpu_resource.buffer == nullptr || gpu_resource.handle == nullptr)
            return false;

        auto global_resource = GpuResourceManager::GetGlobalBuffer(gpu_resource.name);
        if (global_resource != nullptr)
        {
            gpu_resource.buffer = global_resource;
            gpu_resource.buffer->UploadBuffer(gpu_resource.handle);
            continue;
        }
        
        auto it = std::ranges::find_if(material_data, [&gpu_resource](const std::shared_ptr<IMaterialData> &material_data) {
            return material_data->parameter.name == gpu_resource.name;
        });

        if (it == material_data.end())
            continue;

        const auto &data = *it;
        if (!data->is_dirty)
            continue;
        
        if (data->BufferType() == kBufferType_Texture2D)
        {
            gpu_resource.buffer = TextureCollection::GetTexture(*static_cast<AssetPtr<Texture2D> *>(data->Data()));
            if (gpu_resource.buffer == nullptr)
                return false;
            gpu_resource.buffer->UploadBuffer(gpu_resource.handle);
            continue;
        }

        gpu_resource.buffer->UpdateBuffer(data->Data());
    }

    return true;
}

bool GpuResourceGroup::SetBufferToDescriptorTable()
{
    if (!m_is_dirty_)
        return true;

    auto itr = 0;
    const auto handles = DescriptorHeap::AllocateLinedUp(m_gpu_resources_.size());
    for (auto &gpu_resource : m_gpu_resources_)
    {
        if (gpu_resource.buffer == nullptr)
            return false;

        const auto handle = handles[itr];
        gpu_resource.buffer->UploadBuffer(handle);
        gpu_resource.handle = handle;

        ++itr;
    }
    
    m_is_dirty_ = false;

    return true;
}

std::shared_ptr<BufferBase> GpuResourceGroup::FindBufferWithName(const std::string &name)
{
    const auto it = std::ranges::find_if(m_gpu_resources_, [&name](const GpuResource &material_data) {
        return material_data.name == name;
    });

    if (it == m_gpu_resources_.end())
        return {};

    return it->buffer;
}
}