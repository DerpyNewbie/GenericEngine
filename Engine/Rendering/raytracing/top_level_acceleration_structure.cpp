#include "pch.h"
#include "top_level_acceleration_structure.h"

#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
TopLevelAccelerationStructure::TopLevelAccelerationStructure()
{
    auto upload_heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto buff_desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * kMaxInstances);
    RenderEngine::Device()->CreateCommittedResource(
        &upload_heap_prop,
        D3D12_HEAP_FLAG_NONE,
        &buff_desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_instance_desc_buffer_)
    );

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
    inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.InstanceDescs = m_instance_desc_buffer_->GetGPUVirtualAddress();
    inputs.NumDescs = kMaxInstances;
    inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
    RenderEngine::DxrDevice()->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

    info.ScratchDataSizeInBytes = (info.ScratchDataSizeInBytes + 255) & ~255;
    
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

void TopLevelAccelerationStructure::Update(const std::vector<D3D12_RAYTRACING_INSTANCE_DESC> &instance_descs) const
{
    //FIXME : メモリが限界突破する可能性あり
    void *mapped_data = nullptr;
    m_instance_desc_buffer_->Map(0, nullptr, &mapped_data);
    memcpy(mapped_data, instance_descs.data(), sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * instance_descs.size());
    m_instance_desc_buffer_->Unmap(0, nullptr);

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
    inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.InstanceDescs = m_instance_desc_buffer_->GetGPUVirtualAddress();
    inputs.NumDescs = instance_descs.size();
    inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC build_desc = {};
    build_desc.Inputs = inputs;
    build_desc.DestAccelerationStructureData = m_result_buffer_->GetGPUVirtualAddress();
    build_desc.ScratchAccelerationStructureData = m_scratch_buffer_->GetGPUVirtualAddress();

    auto dxr_command_list = RenderEngine::DxrCommandList();
    dxr_command_list->BuildRaytracingAccelerationStructure(&build_desc, 0, nullptr);

    const auto tlas_barrier = CD3DX12_RESOURCE_BARRIER::UAV(m_result_buffer_.Get());
    dxr_command_list->ResourceBarrier(1, &tlas_barrier);
}

D3D12_GPU_VIRTUAL_ADDRESS TopLevelAccelerationStructure::GetGPUVirtualAddress() const
{
    return m_result_buffer_->GetGPUVirtualAddress();
}
}