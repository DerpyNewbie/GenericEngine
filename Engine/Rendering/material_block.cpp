#include "pch.h"
#include "material_block.h"

void engine::MaterialBlock::OnConstructed()
{
    Object::OnConstructed();
    std::pair<std::string, std::shared_ptr<Texture2D>> diffuse_tex = {"diffuse", nullptr};
    params_tex2d.emplace_back(diffuse_tex);
}

void engine::MaterialBlock::CreateBuffer()
{
    params_float_buffer.Initialize(params_float.size());
    params_int_buffer.Initialize(params_int.size());
    params_vec2_buffer.Initialize(params_vec2.size());
    params_vec3_buffer.Initialize(params_vec3.size());
    params_vec4_buffer.Initialize(params_vec4.size());
    params_mat4_buffer.Initialize(params_mat4.size());
}

void engine::MaterialBlock::UpdateBuffer()
{
    params_float_buffer.Upload(GetValues(params_float));
    params_int_buffer.Upload(GetValues(params_int));
    params_vec2_buffer.Upload(GetValues(params_vec2));
    params_vec3_buffer.Upload(GetValues(params_vec3));
    params_vec4_buffer.Upload(GetValues(params_vec4));
    params_mat4_buffer.Upload(GetValues(params_mat4));
}