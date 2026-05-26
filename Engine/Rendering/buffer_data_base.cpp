#include "pch.h"
#include "buffer_data_base.h"

namespace engine
{
BufferDataBase::BufferDataBase(const ShaderParameter &param): parameter(std::move(param))
{}
}

CEREAL_REGISTER_TYPE(engine::BufferDataBase)