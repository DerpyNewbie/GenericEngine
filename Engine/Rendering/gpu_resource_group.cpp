#include "pch.h"
#include "gpu_resource_group.h"

namespace engine
{
int *ShaderDataIndex::GetLengthField(kParameterBufferType type)
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

int ShaderDataIndex::GetLength(const kParameterBufferType type) const
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

int ShaderDataIndex::GetOffset(kParameterBufferType type) const
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

void GpuResourceGroup::Insert(BufferIsExternalPair buffer_pair)
{
    auto data = buffer_pair.first.first;

    const auto buffer_type = data->BufferType();
    data->CreateBuffer();
    buffers.insert(End(buffer_type), {data, nullptr});
    data->is_dirty = false;

    const auto field = m_shader_index_.GetLengthField(buffer_type);
    ++(*field);
}

bool GpuResourceGroup::Empty(kParameterBufferType buffer_type)
{
    return m_shader_index_.GetLength(buffer_type) == 0;
}

std::vector<GpuResourceGroup::BufferHandlePair>::iterator GpuResourceGroup::Begin(kParameterBufferType buffer_type)
{
    const auto buffer_offset = m_shader_index_.GetOffset(buffer_type);
    return buffers.begin() + buffer_offset;
}

std::vector<GpuResourceGroup::BufferHandlePair>::iterator GpuResourceGroup::End(kParameterBufferType buffer_type)
{
    return Begin(buffer_type) + m_shader_index_.GetLength(buffer_type);
}

void GpuResourceGroup::UpdateBuffer(const std::shared_ptr<MaterialBlock> &material_block)
{
    for (auto &[buffer_pair, handle] : buffers)
    {
        auto &buffer = buffer_pair.first;
        if (buffer_pair.second)
        {
            // external buffer, do not update
            continue;
        }

        for (auto &material_data_pair : material_block->m_material_data_)
        {
            auto &data = material_data_pair.data;
            if (data->BufferType() == buffer->BufferType())
            {
                if (data->is_dirty)
                {
                    data->UpdateBuffer();
                }
                break;
            }
        }
    }

}
}