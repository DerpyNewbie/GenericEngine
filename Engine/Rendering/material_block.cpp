#include "pch.h"
#include "material_block.h"
#include "MaterialData.h"

std::shared_ptr<engine::IMaterialData> CreateMaterialData(std::string type_hint)
{
    if (type_hint == "int")
        return std::make_shared<engine::MaterialData<int>>();
    if (type_hint == "float")
        return std::make_shared<engine::MaterialData<float>>();
    if (type_hint == "Texture2D")
        return std::make_shared<engine::MaterialData<std::weak_ptr<Texture2D>>>();
}

void engine::MaterialBlock::OnInspectorGui()
{
    //implement
}

void engine::MaterialBlock::OnConstructed()
{
    Object::OnConstructed();
}
std::shared_ptr<DescriptorHandle> engine::MaterialBlock::GetDescriptorHandle(
    kShaderType shader_type, kParameterBufferType param_buffer)
{
    return parameters[shader_type][param_buffer].begin()->second->UploadBuffer();
}
std::weak_ptr<engine::IMaterialData> engine::MaterialBlock::FindMaterialDataFromName(std::string name)
{
    for (int shader_type = 0; shader_type < kShaderType_Count; ++shader_type)
        for (int params_type = 0; params_type < kParameterBufferType_Count; ++params_type)
            for (auto [key,value]: parameters[shader_type][params_type])
                if (name == key.name)
                    return value;
}
void engine::MaterialBlock::CreateParamsFromShaderParams(std::vector<ShaderParameter> shader_params)
{
    for (int shader_type = 0; shader_type < kShaderType_Count; ++shader_type)
        for (int params_type = 0; params_type < kParameterBufferType_Count; ++params_type)
    std::sort(parameters[shader_type][params_type].begin(), parameters[shader_type][params_type].end(), [](std::pair<ShaderParameter, std::shared_ptr<IMaterialData>> a, std::pair<ShaderParameter, std::shared_ptr<IMaterialData>> b) {
        return a.first.index < b.first.index;
    });
    for (auto shader_param : shader_params)
    {
        auto material_data = CreateMaterialData(shader_param.type_hint);
        material_data->CreateBuffer();
        parameters[shader_param.shader_type][shader_param.buffer_type].emplace_back(
            shader_param, material_data);
    }
}

CEREAL_REGISTER_TYPE(engine::MaterialBlock)