#include "pch.h"
#include "gpu_resource_group.h"

namespace engine
{
int *ShaderDataIndex::GetLengthField(kGpuUploadType type)
{
    switch (type)
    {
        case kParameterBufferType_CBV:
            return &cbv_length;
        case kParameterBufferType_SRV:
            return &srv_length;
        case kParameterBufferType_UAV:
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
        case kParameterBufferType_CBV:
            return cbv_length;
        case kParameterBufferType_SRV:
            return srv_length;
        case kParameterBufferType_UAV:
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
        case kParameterBufferType_UAV:
            offset += srv_length;
        case kParameterBufferType_SRV:
            offset += cbv_length;
        case kParameterBufferType_CBV:
        default:
            break;
    }

    return offset;
}

int ShaderDataIndex::GetFullLength() const
{
    return cbv_length + srv_length + uav_length;
}

void GpuResourceGroup::Insert(const AssetPtr<BufferBase> &buffer, bool is_external)
{
    auto data = buffer;

    const auto buffer_type = data->BufferType();
    data->CreateBuffer();
    m_buffers_.insert(End(buffer_type), {{data, is_external}, nullptr});
    data->is_dirty = false;

    const auto field = m_shader_index_.GetLengthField(buffer_type);
    ++(*field);
}

bool GpuResourceGroup::Empty(const kGpuUploadType buffer_type)
{
    return m_shader_index_.GetLength(buffer_type) == 0;
}

std::vector<GpuResourceGroup::BufferHandlePair>::iterator GpuResourceGroup::Begin(const kGpuUploadType buffer_type)
{
    const auto buffer_offset = m_shader_index_.GetOffset(buffer_type);
    return m_buffers_.begin() + buffer_offset;
}

std::vector<GpuResourceGroup::BufferHandlePair>::iterator GpuResourceGroup::End(const kGpuUploadType buffer_type)
{
    return Begin(buffer_type) + m_shader_index_.GetLength(buffer_type);
}

void GpuResourceGroup::UpdateBuffer(const std::shared_ptr<MaterialBlock> &material_block)
{
    for (auto &buffer_pair : m_buffers_ | std::views::keys)
    {
        const auto &buffer = buffer_pair.first;
        if (buffer_pair.second)
        {
            // external buffer, do not update
            continue;
        }

        for (const auto &data : material_block->material_data)
        {
            if (data->BufferType() == buffer->BufferType())
            {
                if (data->is_dirty)
                {
                    buffer->UpdateBuffer(data->Data());
                    data->is_dirty = false;
                }
                break;
            }
        }
    }

}

void GpuResourceGroup::SetBufferToDescriptorTable()
{
    if (!m_is_dirty_)
        return;

    auto handles = DescriptorHeap::AllocateLinedUp(m_buffers_.size());
    for (size_t i = 0; i < m_buffers_.size(); ++i)
    {
        auto &buffer_pair = m_buffers_[i];
        const auto &buffer = buffer_pair.first.first;
        const auto is_external = buffer_pair.first.second;

        if (is_external)
        {
            // external buffer, do not set to descriptor table
            continue;
        }

        const auto handle = handles[i];
        buffer->UploadBuffer(handle);
        buffer_pair.second = handle;
    }

    m_is_dirty_ = false;
}
}