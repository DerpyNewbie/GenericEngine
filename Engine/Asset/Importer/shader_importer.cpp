#include "pch.h"
#include "shader_importer.h"
#include "logger.h"
#include "Rendering/shader.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"

#include <d3dcompiler.h>

namespace engine
{
bool ShaderImporter::CompileShader(const std::shared_ptr<Shader> &shader, const std::wstring &file_path)
{
    ComPtr<ID3DBlob> error_blob;

    HRESULT hr = D3DCompileFromFile(
        file_path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "vrt",
        "vs_5_0",
        0,
        0,
        &shader->m_pVSBlob,
        &error_blob
        );

    if (FAILED(hr))
    {
        Logger::Error<ShaderImporter>("Failed to Compile Vertex Shader!");
        return false;
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
        &error_blob
        );

    if (FAILED(hr))
    {
        Logger::Error<ShaderImporter>("Failed to Compile Pixel Shader!");
        return false;
    }

    // Add shader variants here if you want to
    return true;
}

bool ShaderImporter::LoadParameters(const std::shared_ptr<Shader> &shader, AssetDescriptor *descriptor) const
{
    const auto json = descriptor->DataStore().GetString("shader_meta");
    if (json.empty())
    {
        Logger::Warn<ShaderImporter>("No shader meta data found for shader '%s'",
                                     descriptor->AssetPath().string().c_str());
        return false;
    }

    using namespace rapidjson;
    Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError())
    {
        Logger::Error<ShaderImporter>("Failed to parse shader meta data for shader '%s'",
                                      descriptor->AssetPath().string().c_str());
        return false;
    }

    const auto parameters_member = doc.FindMember("parameters");
    if (parameters_member == doc.MemberEnd())
    {
        Logger::Warn<ShaderImporter>("No parameters found for shader '%s'", descriptor->AssetPath().string().c_str());
        return false;
    }

    bool params_load_error = false;
    auto &params_value = parameters_member->value;
    auto construct_shader_parameter = [&](const int index, const kShaderType shader_type, auto &object) {
        auto param = std::make_shared<ShaderParameter>();
        param->index = index;
        param->shader_type = shader_type;

        auto name_member = object->FindMember("name");
        if (name_member == object->MemberEnd())
        {
            Logger::Error<ShaderImporter>("No name found for parameter at index %d. which is required.", index);
            params_load_error = true;
        }
        else
        {
            param->name = name_member->value.GetString();
        }

        auto type_member = object->FindMember("type");
        if (type_member == object->MemberEnd())
        {
            Logger::Error<ShaderImporter>("No type found for parameter at index %d. which is required.", index);
            params_load_error = true;
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

    return !params_load_error;
}

std::vector<std::string> ShaderImporter::SupportedExtensions()
{
    return {".hlsl"};
}

bool ShaderImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<Shader>(object) != nullptr;
}

void ShaderImporter::OnImport(AssetDescriptor *ctx)
{
    if (ctx->Guid() == xg::Guid("aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa"))
    {
        ctx->SetMainObject(Shader::GetDefault());
        return;
    }

    const auto shader = Object::Instantiate<Shader>();
    ctx->SetMainObject(shader);

    if (!CompileShader(shader, ctx->AssetPath()))
    {
        ctx->LogImportError("Failed to compile shader!");
        return;
    }

    if (!LoadParameters(shader, ctx))
    {
        ctx->LogImportError("Failed to load shader parameters!");
        return;
    }

    if (!PSOManager::Register(shader, ctx->AssetPath().filename().string()))
    {
        ctx->LogImportError("Failed to register shader to PSOManager!");
    }
}

void ShaderImporter::OnExport(AssetDescriptor *ctx)
{
    const auto shader = std::dynamic_pointer_cast<Shader>(ctx->MainObject());

    using namespace rapidjson;
    auto create_shader_param_obj = [](const std::shared_ptr<ShaderParameter> &param, auto &alloc) {
        auto result = Value(kObjectType);

        auto name_value = Value{};
        name_value.SetString(param->name.c_str(), param->name.size());
        result.AddMember("name", name_value, alloc);

        auto type_value = Value{};
        type_value.SetString(param->type_hint.c_str(), param->type_hint.size());
        result.AddMember("type", type_value, alloc);

        if (!param->display_name.empty())
        {
            auto display_name_value = Value{};
            display_name_value.SetString(param->display_name.c_str(), param->display_name.size());
            result.AddMember("displayName", display_name_value, alloc);
        }
        return result;
    };

    auto create_shader_params_list = [&](auto &params, auto &alloc) {
        auto result = Value(kArrayType);
        for (const auto &param : params)
        {
            result.PushBack(create_shader_param_obj(param, alloc), alloc);
        }
        return result;
    };

    auto create_shader_params_value = [&](auto &alloc) {
        auto vertex_params = std::views::filter(shader->parameters, [](auto &p) {
            return p->shader_type == kShaderType_Vertex;
        });
        auto pixel_params = std::views::filter(shader->parameters, [](auto &p) {
            return p->shader_type == kShaderType_Pixel;
        });

        auto vertex_params_obj = create_shader_params_list(vertex_params, alloc);
        auto pixel_params_obj = create_shader_params_list(pixel_params, alloc);

        Value result(kObjectType);
        result.AddMember("vertex", vertex_params_obj, alloc);
        result.AddMember("pixel", pixel_params_obj, alloc);
        return result;
    };

    Document doc;
    doc.SetObject();
    doc.AddMember("parameters", create_shader_params_value(doc.GetAllocator()), doc.GetAllocator());

    StringBuffer string_buffer;
    Writer writer(string_buffer);
    doc.Accept(writer);

    ctx->DataStore().ClearKeys();
    ctx->DataStore().SetString("shader_meta", string_buffer.GetString());
}
}