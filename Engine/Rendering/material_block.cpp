#include "pch.h"
#include "material_block.h"
#include "MaterialData.h"
#include "gui.h"
#include "world_view_projection.h"
using MaterialFactory =
std::function<std::shared_ptr<engine::IMaterialData>(std::weak_ptr<engine::ShaderParameter> &)>;

std::unordered_map<std::string, MaterialFactory> material_data_map = {
    {"int", [](std::weak_ptr<engine::ShaderParameter> &param) {
        return std::make_shared<engine::MaterialData<int>>(param);
    }},
    {"float", [](std::weak_ptr<engine::ShaderParameter> &param) {
        return std::make_shared<engine::MaterialData<float>>(param);
    }},
    {"__WVP__", [](std::weak_ptr<engine::ShaderParameter> &param) {
        return std::make_shared<engine::MaterialData<WorldViewProjection>>(param);
    }},
    {"vector<Matrix>", [](std::weak_ptr<engine::ShaderParameter> &param) {
        return std::make_shared<engine::MaterialData<std::vector<Matrix>>>(param);
    }},
    {"Texture2D", [](std::weak_ptr<engine::ShaderParameter> &param) {
        return std::make_shared<engine::MaterialData<engine::AssetPtr<Texture2D>>>(param);
    }}
};

std::shared_ptr<engine::IMaterialData> CreateMaterialData(std::weak_ptr<engine::ShaderParameter> shader_param)
{
    auto func = material_data_map[shader_param.lock()->type_hint];
    auto material_data = func(shader_param);
    return material_data;
}

void engine::MaterialBlock::OnInspectorGui()
{
    for (int i = 0; i < kShaderType_Count; ++i)
        for (int j = 0; j < kParameterBufferType_Count; ++j)
            for (auto param : material_datasets[i][j])
                param->OnInspectorGui();
}

void engine::MaterialBlock::OnConstructed()
{
    Object::OnConstructed();
}

std::shared_ptr<DescriptorHandle> engine::MaterialBlock::GetDescriptorHandle(
    kShaderType shader_type, kParameterBufferType param_buffer)
{
    auto params = material_datasets[shader_type][param_buffer];
    if (params.empty())
        return nullptr;

    std::shared_ptr<DescriptorHandle> result = params[0]->UploadBuffer();
    for (int i = 1; i < params.size(); ++i)
        params[i]->UploadBuffer();

    return result;
}

std::weak_ptr<engine::IMaterialData> engine::MaterialBlock::FindMaterialDataFromName(std::string name)
{
    for (int shader_type = 0; shader_type < kShaderType_Count; ++shader_type)
        for (int params_type = 0; params_type < kParameterBufferType_Count; ++params_type)
            for (auto material_data : material_datasets[shader_type][params_type])
                if (name == material_data->GetShaderParam().name)
                    return material_data;
    return {};
}

void engine::MaterialBlock::CreateParamsFromShaderParams(std::vector<std::shared_ptr<ShaderParameter>> shader_params)
{
    for (auto shader_param : shader_params)
    {
        auto material_data = CreateMaterialData(shader_param);
        material_datasets[shader_param->shader_type][material_data->BufferType()].emplace_back(material_data);
    }
}

void engine::MaterialBlock::CreateBuffer()
{
    for (int shader_type = 0; shader_type < kShaderType_Count; ++shader_type)
        for (int params_type = 0; params_type < kParameterBufferType_Count; ++params_type)
            for (auto mat_data : material_datasets[shader_type][params_type])
                mat_data->CreateBuffer();
    m_IsCreate_Buffer_ = true;
}

bool engine::MaterialBlock::IsCreateBuffer()
{
    return m_IsCreate_Buffer_;
}

CEREAL_REGISTER_TYPE(engine::MaterialBlock)