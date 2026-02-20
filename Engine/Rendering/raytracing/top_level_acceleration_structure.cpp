#include "pch.h"
#include "top_level_acceleration_structure.h"

#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
TopLevelAccelerationStructure::TopLevelAccelerationStructure()
{
    D3D12_RAYTRACING_INSTANCE_DESC instance_desc = {};
    float transform[3][4] = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f}
    };

    memcpy(instance_desc.Transform, transform, sizeof(transform));
    instance_desc.InstanceID = 0;
    instance_desc.InstanceMask = 0xFF;
    instance_desc.InstanceContributionToHitGroupIndex = 0;
    instance_desc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
    instance_desc.AccelerationStructure = m_result_buffer_->GetGPUVirtualAddress();

    auto upload_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buff_desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(D3D12_RAYTRACING_INSTANCE_DESC));
    RenderEngine::Device()->CreateCommittedResource(
        &upload_heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &buff_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_instance_desc_buffer_)
    );

    void *mapped_data = nullptr;
    m_instance_desc_buffer_->Map(0, nullptr, &mapped_data);
    memcpy(mapped_data, &instance_desc, sizeof(instance_desc));
    m_instance_desc_buffer_->Unmap(0, nullptr);

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
    inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.InstanceDescs = m_instance_desc_buffer_->GetGPUVirtualAddress();
    inputs.NumDescs = 1;
    inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
    RenderEngine::DxrDevice()->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

    auto default_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto scratch_desc = CD3DX12_RESOURCE_DESC::Buffer(info.ScratchDataSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    RenderEngine::Device()->CreateCommittedResource(
        &default_heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &scratch_desc,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        nullptr,
        IID_PPV_ARGS(&m_scratch_buffer_)
    );

    auto result_desc = CD3DX12_RESOURCE_DESC::Buffer(info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    RenderEngine::Device()->CreateCommittedResource(
        &default_heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &result_desc,
        D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
        nullptr,
        IID_PPV_ARGS(&m_result_buffer_)
    );

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC build_desc = {};
    build_desc.Inputs = inputs;
    build_desc.DestAccelerationStructureData = m_result_buffer_->GetGPUVirtualAddress();
    build_desc.ScratchAccelerationStructureData = m_scratch_buffer_->GetGPUVirtualAddress();

    RenderEngine::DxrCommandList()->BuildRaytracingAccelerationStructure(&build_desc, 0, nullptr);

    auto barrier = CD3DX12_RESOURCE_BARRIER::UAV(m_result_buffer_.Get());
    RenderEngine::DxrCommandList()->ResourceBarrier(1, &barrier);
}
}