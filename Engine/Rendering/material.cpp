#include "pch.h"
#include "material.h"

void engine::Material::OnConstructed()
{
    Object::OnConstructed();
    shared_shader = Instantiate<Shader>();
    shared_material_block = Instantiate<MaterialBlock>();
    
}

void engine::Material::CreateMaterialBlock()
{
    shared_material_block->CreateParamsFromShaderParams(shared_shader->parameters);
}

CEREAL_REGISTER_TYPE(engine::Material)