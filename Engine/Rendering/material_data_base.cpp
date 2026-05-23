#include "pch.h"
#include "material_data_base.h"

namespace engine
{
MaterialDataBase::MaterialDataBase(const ShaderParameter &param): parameter(std::move(param))
{}

void MaterialDataBase::OnDeserialized()
{
    is_dirty = true;
}
}

CEREAL_REGISTER_TYPE(engine::MaterialDataBase)