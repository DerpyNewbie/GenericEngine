#include "pch.h"
#include "gpu_resource_group.h"

#include "gpu_resource_manager.h"

namespace engine
{
void GpuResourceGroup::UpdateConstantBuffer(const GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block)
{
    if (gpu_resource.handle != nullptr)
        gpu_resource.buffer->UploadBuffer(gpu_resource.handle);

    const auto cb_data = material_block->GetConstantBufferData(gpu_resource.name);
    if (cb_data != nullptr && cb_data->is_dirty)
        gpu_resource.buffer->UpdateBuffer(cb_data->Data());
}

void GpuResourceGroup::UpdateStructuredBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block)
{
    auto sb_data = material_block->GetStructuredBufferData(gpu_resource.name);
    if (sb_data == nullptr)
        return;

    if (sb_data->GetIsSizeChanged())
    {
        sb_data->SetIsSizeChanged(false);
        gpu_resource.buffer = std::make_shared<StructuredBuffer>(sb_data->Stride(), sb_data->Count());
        gpu_resource.buffer->CreateBuffer();
        if (gpu_resource.handle != nullptr)
            gpu_resource.buffer->UploadBuffer(gpu_resource.handle);
    }
    if (sb_data->is_dirty)
        gpu_resource.buffer->UpdateBuffer(sb_data->Data());

    gpu_resource.buffer->Transition(sb_data->parameter.is_unordered_access ? D3D12_RESOURCE_STATE_UNORDERED_ACCESS : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void GpuResourceGroup::UpdateTextureBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block)
{
    auto tex_data = material_block->GetTextureBufferData(gpu_resource.name);
    if (tex_data != nullptr && tex_data->is_dirty)
    {
        auto texture = tex_data->Data();
        if (texture == nullptr)
            return;

        gpu_resource.buffer = TextureCollection::LoadTexture(texture);

        if (gpu_resource.handle != nullptr)
            gpu_resource.buffer->UploadBuffer(gpu_resource.handle);
    }

    if (!gpu_resource.buffer->IsValid())
        gpu_resource.buffer->CreateBuffer();

    gpu_resource.buffer->Transition(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void GpuResourceGroup::UpdateUavTextureBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block)
{
    auto tex_data = material_block->GetUavTextureBufferData(gpu_resource.name);
    if (tex_data != nullptr && tex_data->is_dirty)
    {
        auto texture = tex_data->Data();
        if (texture == nullptr)
            return;

        gpu_resource.buffer = TextureCollection::LoadRenderTexture(texture);

        if (gpu_resource.handle != nullptr)
            gpu_resource.buffer->UploadBuffer(gpu_resource.handle);
    }

    if (!gpu_resource.buffer->IsValid())
        gpu_resource.buffer->CreateBuffer();

    gpu_resource.buffer->Transition(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

bool GpuResourceGroup::SetGlobalResource(GpuResource &gpu_resource, bool is_uav)
{
    auto global_resource = GpuResourceManager::GetGlobalBuffer(gpu_resource.name);
    if (global_resource == nullptr)
    {
        return false;
    }

    if (!global_resource->IsValid())
        global_resource->CreateBuffer();

    if (!global_resource->IsValid() || gpu_resource.handle == nullptr)
        return false;
    
    gpu_resource.buffer = global_resource;
    gpu_resource.buffer->UploadBuffer(gpu_resource.handle, is_uav);

    return true;
}

void GpuResourceGroup::Insert(const std::shared_ptr<BufferBase> &buffer, const std::shared_ptr<BufferDataBase> &material_data, kBufferType buffer_type, kGpuUploadType gpu_upload_type)
{
    if (buffer)
        buffer->CreateBuffer();

    GpuResource gpu_resource;
    gpu_resource.name = material_data->parameter.name;
    gpu_resource.buffer = buffer;
    gpu_resource.buffer_type = buffer_type;

    m_gpu_resources_[gpu_upload_type].try_emplace(material_data->parameter.index, gpu_resource);
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

bool GpuResourceGroup::UpdateBuffer(const std::shared_ptr<MaterialBlock> &material_block)
{
    for (int i = 0; i < kGpuBufferType_Count; ++i)
    {
        for (auto &gpu_resource : m_gpu_resources_[i] | std::views::values)
        {
            if (SetGlobalResource(gpu_resource, i == kGpuBufferType_UAV))
                continue;

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
                case kBufferType_UavTexture:
                    UpdateUavTextureBuffer(gpu_resource, material_block);
                    break;
            }
        }
    }

    return true;
}

bool GpuResourceGroup::SetBufferToDescriptorTable()
{
    if (!m_is_dirty_)
        return true;

    for (int i = 0; i < kGpuBufferType_Count; ++i)
    {
        auto it = std::ranges::find_if(m_gpu_resources_[i], [](const std::pair<int, GpuResource> &a) {
            return a.second.handle == nullptr;
        });
        //現在のBufferTypeがemptyまたはすでにhandleが確保されていたら次に進む
        if (m_gpu_resources_[i].empty() || it == m_gpu_resources_[i].end())
            continue;

        auto itr = 0;
        const auto handles = DescriptorHeap::AllocateLinedUp(m_gpu_resources_[i].size());
        for (auto &gpu_resource : m_gpu_resources_[i] | std::views::values)
        {
            //BufferがなかったらSetできないのでHandleをすべて手放して早期リターン
            if (gpu_resource.buffer == nullptr)
            {
                for (auto handle : handles)
                    DescriptorHeap::Free(handle);
                return false;
            }

            const auto handle = handles[itr];
            gpu_resource.buffer->UploadBuffer(handle, kGpuBufferType_UAV == i);
            gpu_resource.handle = handle;

            ++itr;
        }
    }

    m_is_dirty_ = false;

    return true;
}
}