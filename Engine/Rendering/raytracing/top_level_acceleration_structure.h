#pragma once
#include "Rendering/uav_buffer.h"

namespace engine
{
class TopLevelAccelerationStructure
{
    ComPtr<ID3D12Resource> m_instance_desc_buffer_;
    ComPtr<ID3D12Resource> m_scratch_buffer_;
    ComPtr<ID3D12Resource> m_result_buffer_;

public:
    TopLevelAccelerationStructure();
};
}