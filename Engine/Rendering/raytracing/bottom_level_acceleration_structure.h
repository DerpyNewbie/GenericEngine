#pragma once
#include "Rendering/mesh.h"
#include "Rendering/uav_buffer.h"

namespace engine
{
class BottomLevelAccelerationStructure
{
    std::shared_ptr<UavBuffer> m_result_buffer_;
    std::shared_ptr<UavBuffer> m_scratch_buffer_;

public:
    BottomLevelAccelerationStructure(const Mesh *mesh_data);
};
}