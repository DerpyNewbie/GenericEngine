#include "pch.h"
#include "ShaderImporter.h"
#include "logger.h"
#include "Rendering/shader.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"

#include <d3dcompiler.h>

namespace engine
{
bool ShaderImporter::CompileShader(std::shared_ptr<Shader> shader, std::wstring file_path)
{
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompileFromFile(
        file_path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "vrt",
        "vs_5_0",
        0,
        0,
        &shader->m_pVSBlob,
        &errorBlob
        );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            Logger::Error<Shader>("Failed to Compile Vertex Shader!");
            return false;
        }
    }

    hr = D3DCompileFromFile(
        file_path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "pix",
        "ps_5_0",
        0,
        0,
        &shader->m_pPSBlob,
        &errorBlob
        );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            Logger::Error<Shader>("Failed to Compile Pixel Shader!");
            return false;
        }
    }

    // TODO: impl
    return false;
}

void ShaderImporter::LoadParameters(std::shared_ptr<Shader> shader, AssetDescriptor *descriptor)
{
    const auto json = descriptor->GetString("shader_meta");
    if (json.empty())
    {
        Logger::Warn<Shader>("No shader meta data found for shader '%s'", descriptor->path_hint.string().c_str());
        return;
    }

    using namespace rapidjson;
    Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError())
    {
        Logger::Error<Shader>("Failed to parse shader meta data for shader '%s'",
                              descriptor->path_hint.string().c_str());
        return;
    }

    auto parameters_member = doc.FindMember("parameters");
    if (parameters_member == doc.MemberEnd())
    {
        Logger::Warn<Shader>("No parameters found for shader '%s'", descriptor->path_hint.string().c_str());
    }
    else
    {
        auto &params_value = parameters_member->value;
        auto construct_shader_parameter = [](const int index, const kShaderType shader_type, auto &object) {
            auto param = std::make_shared<ShaderParameter>();
            param->index = index;
            param->shader_type = shader_type;

            auto name_member = object->FindMember("name");
            if (name_member == object->MemberEnd())
            {
                Logger::Error<Shader>("No name found for parameter at index %d. which is required.", index);
            }
            else
            {
                param->name = name_member->value.GetString();
            }

            auto type_member = object->FindMember("type");
            if (type_member == object->MemberEnd())
            {
                Logger::Error<Shader>("No type found for parameter at index %d. which is required.", index);
            }
            else
            {
                param->type_hint = type_member->value.GetString();
            }

            auto display_name_member = object->FindMember("displayName");
            if (display_name_member == object->MemberEnd())
            {
                param->display_name = std::string();
            }
            else
            {
                param->display_name = display_name_member->value.GetString();
            }

            return param;
        };
        auto read_shader_params = [&](const kShaderType shader_type, const char *name) {
            std::vector<std::shared_ptr<ShaderParameter>> params;
            const auto it = params_value.FindMember(name);
            if (it == params_value.MemberEnd())
            {
                return params;
            }

            int index = 0;
            const auto params_array = it->value.GetArray();
            for (auto i = params_array.begin(); i != params_array.end(); ++i, ++index)
            {
                params.emplace_back(construct_shader_parameter(index, shader_type, i));
            }

            return params;
        };

        auto vertex_params = read_shader_params(kShaderType_Vertex, "vertex");
        auto pixel_params = read_shader_params(kShaderType_Pixel, "pixel");

        shader->parameters.insert(shader->parameters.end(), vertex_params.begin(), vertex_params.end());
        shader->parameters.insert(shader->parameters.end(), pixel_params.begin(), pixel_params.end());
    }

}

std::vector<std::string> ShaderImporter::SupportedExtensions()
{
    return {".hlsl"};
}

std::shared_ptr<Object> ShaderImporter::Import(AssetDescriptor *asset)
{
    auto shader = Object::Instantiate<Shader>(asset->guid);
    if (asset->guid == xg::Guid("aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa"))
    {
        shader = Shader::GetDefault();
        return shader;
    }

    CompileShader(shader, asset->path_hint);
    LoadParameters(shader, asset);
    g_PSOManager.Register(shader, asset->path_hint.filename().string());
    return shader;
}
}