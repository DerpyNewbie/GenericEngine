#include "pch.h"
#include "shader.h"
#include "gui.h"
#include "crs_bridge.h"
#include "CaboRenderingSystem/render_engine.h"

namespace engine
{
namespace
{
std::string ReadText(const std::wstring &file_path)
{
    std::ifstream file(file_path);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

kConstantBufferDataType ToDataType(const CRS::ShaderVariableType type)
{
    switch (type)
    {
    case CRS::ShaderVariableType::Int:
    case CRS::ShaderVariableType::UInt:
        return kConstantBufferDataType::kConstantBufferDataTypeInt;
    case CRS::ShaderVariableType::Float:
        return kConstantBufferDataType::kConstantBufferDataType_Float;
    case CRS::ShaderVariableType::Float2:
        return kConstantBufferDataType::kConstantBufferDataType_Vector2;
    case CRS::ShaderVariableType::Float3:
        return kConstantBufferDataType::kConstantBufferDataType_Vector3;
    case CRS::ShaderVariableType::Float4:
        return kConstantBufferDataType::kConstantBufferDataType_Color;
    case CRS::ShaderVariableType::Float4x4:
        return kConstantBufferDataType::kConstantBufferDataType_Matrix;
    default:
        return kConstantBufferDataType::kConstantBufferDataType_Unknown;
    }
}

/// <summary>
/// コンパイル結果のリフレクションから、Materialのパラメーター（b1の定数とspace0のテクスチャ）を作ります。
/// </summary>
std::vector<ShaderParameter> ReadMaterialParameters(const CrsShaderProgram &program)
{
    std::vector<ShaderParameter> result;
    const auto add = [&](ShaderParameter param) {
        if (std::ranges::none_of(result, [&](const ShaderParameter &p) { return p.name == param.name; }))
            result.push_back(std::move(param));
    };

    for (const auto *shader : {&program.vertex, &program.pixel})
    {
        if (!shader->IsValid())
            continue;

        const auto &reflection = shader->Reflection();
        for (const auto &cb : reflection.constant_buffers)
        {
            if (cb.space != 0)
                continue;

            ShaderParameter param = {
                static_cast<int>(cb.slot), cb.name, cb.name, kBufferType_ConstantBuffer, false, cb.size
            };
            for (const auto &variable : cb.variables)
                param.variables.push_back({variable.name, ToDataType(variable.type)});
            add(std::move(param));
        }

        for (const auto &resource : reflection.resources)
        {
            if (resource.space == 0 && resource.kind == CRS::ShaderResourceKind::Texture)
                add({static_cast<int>(resource.slot), resource.name, resource.name, kBufferType_Texture2D});
        }
    }
    return result;
}
}

bool Shader::Compile(const std::wstring &file_path, std::string &error_message)
{
    const auto source = ReadText(file_path);
    if (source.empty())
    {
        error_message = "Failed to read shader file";
        return false;
    }

    if (source.find("VSMain") == std::string::npos)
    {
        error_message = "Entry point VSMain was not found. Shaders must define VSMain (and PSMain / VSShadow).";
        return false;
    }

    auto &device = CrsBridge::Crs().Device();
    auto program = std::make_shared<CrsShaderProgram>();
    program->vertex = CRS::Shader(device, {file_path, L"VSMain", CRS::ShaderStage::Vertex, {}});
    if (!program->vertex.IsValid())
    {
        error_message = "Failed to compile VSMain (see the log for details)";
        return false;
    }

    if (source.find("PSMain") != std::string::npos)
    {
        program->pixel = CRS::Shader(device, {file_path, L"PSMain", CRS::ShaderStage::Pixel, {}});
        if (!program->pixel.IsValid())
        {
            error_message = "Failed to compile PSMain (see the log for details)";
            return false;
        }
    }

    if (source.find("VSShadow") != std::string::npos)
    {
        program->shadow_vertex = CRS::Shader(device, {file_path, L"VSShadow", CRS::ShaderStage::Vertex, {}});
        if (!program->shadow_vertex.IsValid())
        {
            error_message = "Failed to compile VSShadow (see the log for details)";
            return false;
        }
    }

    m_program_ = std::move(program);
    m_is_ui_shader_ = source.find("crs_ui.hlsli") != std::string::npos;
    m_shader_settings_.ApplySourceTags(source);

    // パラメーターを読み直す。表示名は引き継ぐ
    auto new_parameters = ReadMaterialParameters(*m_program_);
    for (auto &parameter : new_parameters)
    {
        const auto old = std::ranges::find(parameters, parameter);
        if (old != parameters.end())
            parameter.display_name = old->display_name;
    }
    parameters = std::move(new_parameters);
    return true;
}

void Shader::DrawShaderSettings()
{
    ImGui::Combo("ZTest", &m_shader_settings_.z_test, ZTestNames, IM_ARRAYSIZE(ZTestNames));
    ImGui::Combo("ZWrite", &m_shader_settings_.z_write, ZWriteNames, IM_ARRAYSIZE(ZWriteNames));
    ImGui::Combo("Cull", &m_shader_settings_.cull, CullNames, IM_ARRAYSIZE(CullNames));
    ImGui::Combo("Blend", &m_shader_settings_.blend_mode, BlendModeNames, IM_ARRAYSIZE(BlendModeNames));
}

void Shader::OnInspectorGui()
{
    if (ImGui::CollapsingHeader("ShaderSettings"))
    {
        ImGui::Indent();
        DrawShaderSettings();
        ImGui::Unindent();
    }

    ImGui::PushID("Shader Parameters");
    if (ImGui::CollapsingHeader("Shader Parameters"))
    {
        ImGui::Indent();
        int index = 0;
        for (auto &param : parameters)
        {
            ImGui::PushID(&param);
            const auto should_show = ImGui::CollapsingHeader(("Parameter " + std::to_string(index++)).c_str());
            if (should_show)
            {
                ImGui::Indent();
                Gui::ReadOnlyStringField("Index", std::to_string(param.index));
                Gui::ReadOnlyStringField("Name", param.name);
                ImGui::InputText("Display Name", &param.display_name);
                ImGui::Unindent();
            }
            ImGui::PopID();
        }
        ImGui::Unindent();
    }
    ImGui::PopID();
}

ShaderSettings Shader::ShaderSettings() const
{
    return m_shader_settings_;
}
}

CEREAL_REGISTER_TYPE(engine::Shader)
