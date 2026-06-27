#pragma once
#include "buffer_base.h"
#include "shader_parameter.h"
#include "CabotEngine/Graphics/ConstantBuffer.h"

namespace engine
{
struct GpuResource
{
    std::string name;
    std::shared_ptr<BufferBase> buffer;
    std::shared_ptr<DescriptorHandle> handle;
    kBufferType buffer_type;
};
}