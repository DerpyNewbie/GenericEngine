#include "pch.h"
#include "gpu_resource_group.h"

#include "gpu_resource_manager.h"
#include "CabotEngine/Graphics/ByteAddressBuffer.h"
#include "CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
void GpuResourceGroup::UpdateConstantBuffer(const GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block)
{
    if (gpu_resource.handle != nullptr)
        gpu_resource.buffer->UploadBuffer(gpu_resource.handle);

    const auto cb_data = material_block->GetConstantBufferData(gpu_resource.name);
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
    if (tex_data->is_dirty)
    {
        auto texture = tex_data->Data();
        if (texture == nullptr)
            return;

        gpu_resource.buffer = TextureCollection::GetTexture(texture);

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
    if (tex_data->is_dirty)
    {
        auto texture = tex_data->Data();
        if (texture == nullptr)
            return;

        gpu_resource.buffer = TextureCollection::GetRenderTexture(texture);

        if (gpu_resource.handle != nullptr)
            gpu_resource.buffer->UploadBuffer(gpu_resource.handle);
    }

    if (!gpu_resource.buffer->IsValid())
        gpu_resource.buffer->CreateBuffer();

    gpu_resource.buffer->Transition(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void GpuResourceGroup::UpdateByteAddressBuffer(GpuResource &gpu_resource, const std::shared_ptr<MaterialBlock> &material_block)
{
    auto byte_address_data = material_block->GetByteAddressBufferData(gpu_resource.name);
    if (byte_address_data->is_size_changed)
    {
        byte_address_data->is_size_changed = false;
        gpu_resource.buffer = std::make_shared<ByteAddressBuffer>(byte_address_data->Count());
        gpu_resource.buffer->CreateBuffer();
        if (gpu_resource.handle != nullptr)
            gpu_resource.buffer->UploadBuffer(gpu_resource.handle);
    }
    if (byte_address_data->is_dirty)
        gpu_resource.buffer->UpdateBuffer(byte_address_data->Data());

    gpu_resource.buffer->Transition(byte_address_data->parameter.is_unordered_access ? D3D12_RESOURCE_STATE_UNORDERED_ACCESS : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
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

void GpuResourceGroup::SetBuffer(const std::string &name, const std::shared_ptr<BufferBase> &buffer)
{
    if (buffer == nullptr)
        return;

    bool find = false;
    GpuResource find_gpu_resource;
    for (auto &gpu_resources : m_gpu_resources_)
    {
        auto it = std::ranges::find_if(gpu_resources,
                                       [&name](auto &gpu_resource) {
                                           return gpu_resource.second.name == name;
                                       });

        if (it != gpu_resources.end())
        {
            find = true;
            find_gpu_resource = it->second;
            break;
        }
    }

    if (!find)
        return;

    if (!buffer->IsValid())
        buffer->CreateBuffer();

    find_gpu_resource.buffer = buffer;
}

std::shared_ptr<BufferBase> GpuResourceGroup::GetBuffer(const std::string &name)
{
    for (auto &gpu_resources : m_gpu_resources_)
    {
        auto it = std::ranges::find_if(gpu_resources,
                                       [&name](auto &gpu_resource) {
                                           return gpu_resource.second.name == name;
                                       });

        if (it != gpu_resources.end())
            return it->second.buffer;
    }

    return nullptr;
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
                case kBufferType_ByteAddressBuffer:
                    UpdateByteAddressBuffer(gpu_resource, material_block);
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
        if (m_gpu_resources_[i].empty() || it == m_gpu_resources_[i].end())
            continue;

        auto itr = 0;
        const auto handles = DescriptorHeap::AllocateLinedUp(m_gpu_resources_[i].size());
        for (auto &gpu_resource : m_gpu_resources_[i] | std::views::values)
        {
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

void GpuResourceGroup::WaitUavWrite()
{
    for (auto &gpu_resource : m_gpu_resources_[kGpuBufferType_UAV] | std::views::values)
    {
        D3D12_RESOURCE_BARRIER uavBarrier = {};
        uavBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        uavBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        uavBarrier.UAV.pResource = gpu_resource.buffer->Resource();
        RenderEngine::CommandList()->ResourceBarrier(1, &uavBarrier);
    }
}
}
