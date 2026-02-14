#include "bottom_level_acceleration_structure.h"

#include <directx/d3d12.h>

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure(const engine::Mesh *mesh_data)
{
    D3D12_RAYTRACING_GEOMETRY_DESC geom_desc = {};
    geom_desc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geom_desc.Triangles.VertexBuffer.StartAddress
}