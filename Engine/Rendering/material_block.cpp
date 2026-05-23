#include "pch.h"
#include "material_block.h"
#include "material_data.h"

namespace
{
using namespace engine;

std::shared_ptr<MaterialDataBase> CreateMaterialData(const ShaderParameter &shader_param)
{
    switch (shader_param.buffer_type)
    {
        case kBufferType_ConstantBuffer:
            return std::make_shared<ConstantBufferData>(shader_param);
        case kBufferType_StructuredBuffer:
            return std::make_shared<StructuredBufferData>(shader_param);
        case kBufferType_Texture2D:
            return std::make_shared<TextureBufferData>(shader_param);
    }
}
}

namespace engine
{

void MaterialBlock::OnInspectorGui()
{
    for (auto &data : m_constant_buffer_data_ | std::views::values)
    {
        ImGui::PushID(data.get());
        data->OnInspectorGui();
        ImGui::PopID();
    }
    for (auto &data : m_structured_buffer_data_ | std::views::values)
    {
        ImGui::PushID(data.get());
        data->OnInspectorGui();
        ImGui::PopID();
    }
    for (auto &data : m_texture_buffer_data_ | std::views::values)
    {
        ImGui::PushID(data.get());
        data->OnInspectorGui();
        ImGui::PopID();
    }
}

std::shared_ptr<ConstantBufferData> MaterialBlock::GetConstantBufferData(const std::string &name)
{
    return m_constant_buffer_data_.find(name)->second;
}

std::shared_ptr<StructuredBufferData> MaterialBlock::GetStructuredBufferData(const std::string &name)
{
    return m_structured_buffer_data_.find(name)->second;
}

std::shared_ptr<TextureBufferData> MaterialBlock::GetTextureBufferData(const std::string &name)
{
    return m_texture_buffer_data_.find(name)->second;
}

void MaterialBlock::LoadShaderParameters(
    const std::vector<ShaderParameter> &shader_params,
    const std::vector<std::shared_ptr<MaterialDataBase>> &resource_material_data
)
{

    //TODO : パラメーターの順番が正しいとは限らないので、パラメーターのインデックス見に行った方がいいかも。 要検証。
    for (auto &param : shader_params)
    {
        bool found = false;
        if (!resource_material_data.empty())
        {
            for (auto res_data : resource_material_data)
            {
                if (param.name == res_data->parameter.name)
                {
                    material_data.emplace_back(res_data);
                    found = true;
                    break;
                }
            }
        }

        if (!found)
        {
            const auto data = CreateMaterialData(param);

            if (data != nullptr)
            {
                material_data.emplace_back(data);
            }
        }
    }
}

std::shared_ptr<MaterialDataBase> MaterialBlock::FindMaterialDataByName(const std::string &name)
{
    const auto it = std::ranges::find_if(material_data, [&name](const std::shared_ptr<MaterialDataBase> &data) {
        return data->parameter.name == name;
    });
    if (it != material_data.end())
    {
        return *it;
    }

    return {};
}

bool MaterialBlock::IsDirty()
{
    const auto it = std::ranges::find_if(material_data, [](const std::shared_ptr<MaterialDataBase> &data) {
        return data->is_dirty;
    });

    if (it != material_data.end())
    {
        return true;
    }

    return false;
}
}

CEREAL_REGISTER_TYPE(engine::MaterialBlock)