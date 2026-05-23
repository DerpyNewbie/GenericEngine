#include "pch.h"
#include "material_block.h"

#include <assimp/material.h>

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

void MaterialBlock::LoadShaderParameters(const std::vector<ShaderParameter> &shader_params)
{
    //TODO : パラメーターの順番が正しいとは限らないので、パラメーターのインデックス見に行った方がいいかも。 要検証。
    for (auto &param : shader_params)
    {
        const auto data = CreateMaterialData(param);

        if (data == nullptr)
            continue;

        switch (param.buffer_type)
        {
            case kBufferType_ConstantBuffer:
                m_constant_buffer_data_.try_emplace(param.name, std::static_pointer_cast<ConstantBufferData>(data));
                break;
            case kBufferType_StructuredBuffer:
                m_structured_buffer_data_.try_emplace(param.name, std::static_pointer_cast<StructuredBufferData>(data));
                break;
            case kBufferType_Texture2D:
                m_texture_buffer_data_.try_emplace(param.name, std::static_pointer_cast<TextureBufferData>(data));
                break;
        }
    }
}
}

CEREAL_REGISTER_TYPE(engine::MaterialBlock)