#include "pch.h"
#include "material_block.h"
#include "MaterialData.h"
#include "world_view_projection.h"

std::shared_ptr<engine::IMaterialData> CreateMaterialData(std::string type_hint)
{
    if (type_hint == "int")
        return std::make_shared<engine::MaterialData<int>>();
    if (type_hint == "float")
        return std::make_shared<engine::MaterialData<float>>();
    if (type_hint == "Texture2D")
        return std::make_shared<engine::MaterialData<engine::AssetPtr<Texture2D>>>();
    if (type_hint == "vector<Matrix>")
        return std::make_shared<engine::MaterialData<std::vector<Matrix>>>();
    //engine
    if (type_hint == "__WVP__")
        return std::make_shared<engine::MaterialData<WorldViewProjection>>();
}

void engine::MaterialBlock::OnInspectorGui()
{

}

void engine::MaterialBlock::OnConstructed()
{
    Object::OnConstructed();
}

std::shared_ptr<DescriptorHandle> engine::MaterialBlock::GetDescriptorHandle(
    kShaderType shader_type, kParameterBufferType param_buffer)
{
    auto params = parameters[shader_type][param_buffer];
    if (params.empty())
        return nullptr;

    std::shared_ptr<DescriptorHandle> result = params[0].second->UploadBuffer();
    for (int i = 1; i < params.size(); ++i)
        params[i].second->UploadBuffer();

    return result;
}

std::weak_ptr<engine::IMaterialData> engine::MaterialBlock::FindMaterialDataFromName(std::string name)
{
    for (int shader_type = 0; shader_type < kShaderType_Count; ++shader_type)
        for (int params_type = 0; params_type < kParameterBufferType_Count; ++params_type)
            for (auto [key,value] : parameters[shader_type][params_type])
                if (name == key.name)
                    return value;
    return std::weak_ptr<engine::IMaterialData>();
}

void engine::MaterialBlock::CreateParamsFromShaderParams(std::vector<ShaderParameter> shader_params)
{
    for (int shader_type = 0; shader_type < kShaderType_Count; ++shader_type)
        for (int params_type = 0; params_type < kParameterBufferType_Count; ++params_type)
            std::sort(parameters[shader_type][params_type].begin(), parameters[shader_type][params_type].end(),
                      [](std::pair<ShaderParameter, std::shared_ptr<IMaterialData>> a,
                         std::pair<ShaderParameter, std::shared_ptr<IMaterialData>> b) {
                          return a.first.index < b.first.index;
                      });
    for (auto shader_param : shader_params)
    {
        auto material_data = CreateMaterialData(shader_param.type_hint);
        parameters[shader_param.shader_type][material_data->BufferType()].emplace_back(
            shader_param, material_data);

    }
}

void engine::MaterialBlock::CreateBuffer()
{
    for (int shader_type = 0; shader_type < kShaderType_Count; ++shader_type)
        for (int params_type = 0; params_type < kParameterBufferType_Count; ++params_type)
            for (auto mat_data : parameters[shader_type][params_type] | std::views::values)
                mat_data->CreateBuffer();
    m_IsCreate_Buffer_ = true;
}

bool engine::MaterialBlock::IsCreateBuffer()
{
    return m_IsCreate_Buffer_;
}

CEREAL_REGISTER_TYPE(engine::MaterialBlock)