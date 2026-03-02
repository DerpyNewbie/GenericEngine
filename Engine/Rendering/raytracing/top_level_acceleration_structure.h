#pragma once
#include <directx/d3d12.h>

#include "Components/mesh_renderer.h"
#include "Rendering/uav_buffer.h"

namespace engine
{
class TopLevelAccelerationStructure
{
    static constexpr UINT kMaxInstances = 1000;
    
    ComPtr<ID3D12Resource> m_instance_desc_buffer_;
    ComPtr<ID3D12Resource> m_scratch_buffer_;
    ComPtr<ID3D12Resource> m_result_buffer_;

public:
    explicit TopLevelAccelerationStructure();

    void Update(const std::vector<D3D12_RAYTRACING_INSTANCE_DESC> &instance_descs) const;
    
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;
};
}