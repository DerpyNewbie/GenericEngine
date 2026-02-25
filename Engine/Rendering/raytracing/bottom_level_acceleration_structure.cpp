#include "pch.h"
#include "bottom_level_acceleration_structure.h"

#include "Rendering/vertex.h"
#include "Rendering/CabotEngine/Graphics/RenderEngine.h"

namespace engine
{
D3D12_GPU_VIRTUAL_ADDRESS BottomLevelAccelerationStructure::GetGPUVirtualAddress()
{
    return m_result_buffer_->GetAddress();
}

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure(const Mesh *mesh_data)
{
    D3D12_RAYTRACING_GEOMETRY_DESC geom_desc = {};
    geom_desc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geom_desc.Triangles.VertexBuffer.StartAddress = mesh_data->vertex_buffer->GetGPUVirtualAddress();//TODO : どうにかしといて
    geom_desc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);
    geom_desc.Triangles.VertexCount = mesh_data->vertices.size();
    geom_desc.Triangles.IndexBuffer = mesh_data->index_buffers[0]->GetGPUVirtualAddress();
    geom_desc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
    geom_desc.Triangles.IndexCount = mesh_data->indices.size();
    geom_desc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
    geom_desc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
    inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    inputs.NumDescs = 1;
    inputs.pGeometryDescs = &geom_desc;
    inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};
    RenderEngine::DxrDevice()->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &info);

    m_scratch_buffer_ = std::make_shared<UavBuffer>(info.ScratchDataSizeInBytes, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    m_result_buffer_ = std::make_shared<UavBuffer>(info.ResultDataMaxSizeInBytes, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE);

    m_scratch_buffer_->CreateBuffer();
    m_result_buffer_->CreateBuffer();
    
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC build_desc = {};
    build_desc.Inputs = inputs;
    build_desc.DestAccelerationStructureData = m_result_buffer_->GetAddress();
    build_desc.ScratchAccelerationStructureData = m_scratch_buffer_->GetAddress();

    RenderEngine::DxrCommandList()->BuildRaytracingAccelerationStructure(&build_desc, 0, nullptr);

    auto barrier = CD3DX12_RESOURCE_BARRIER::UAV(m_result_buffer_->GetResource());
    RenderEngine::CommandList()->ResourceBarrier(1, &barrier);
}
}