#pragma once
#include <directx/d3d12.h>

#include "Rendering/uav_buffer.h"

namespace engine
{
class TopLevelAccelerationStructure
{
    ComPtr<ID3D12Resource> m_instance_desc_buffer_;
    ComPtr<ID3D12Resource> m_scratch_buffer_;
    ComPtr<ID3D12Resource> m_result_buffer_;

public:
    explicit TopLevelAccelerationStructure(D3D12_GPU_VIRTUAL_ADDRESS blas_address);
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;
};
}