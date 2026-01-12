#include "pch.h"
#include "shader_importer.h"
#include "logger.h"
#include "Rendering/shader.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"

#include <d3dcompiler.h>

#include "serializer.h"

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
        &shader->m_vs_blob_,
        &error_blob
    );

    if (FAILED(hr))
    {
        if (error_blob)
        {
            Logger::Error<ShaderImporter>(
                reinterpret_cast<const char*>(error_blob->GetBufferPointer())
            );
        }
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
        &shader->m_ps_blob_,
        &error_blob
    );

    if (FAILED(hr))
    {
        if (error_blob)
        {
            Logger::Error<ShaderImporter>(
                reinterpret_cast<const char*>(error_blob->GetBufferPointer())
            );
        }
        return false;
    }

    // Add shader variants here if you want to
    return true;
}

bool ShaderImporter::LoadOldParameters(const std::shared_ptr<Shader> &shader, AssetDescriptor *descriptor) const
{
    auto json = descriptor->DataStore().GetString("shader_meta");
    if (json.empty())
    {
        Logger::Warn<ShaderImporter>(
            "No shader meta data found for shader '%s'",
            descriptor->AssetPath().string().c_str()
        );
        return false;
    }

    using namespace rapidjson;
    Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError())
    {
        Logger::Error<ShaderImporter>(
            "Failed to parse shader meta data for shader '%s'",
            descriptor->AssetPath().string().c_str()
        );
        return false;
    }

    const PersistentDataStore data_store{&doc, &doc};
    const auto shader_settings = data_store.GetDataStore("shader_settings");
    shader->m_shader_settings_.z_test = shader_settings.GetInt("z_test");
    shader->m_shader_settings_.z_write = shader_settings.GetInt("z_write");
    shader->m_shader_settings_.cull = shader_settings.GetInt("cull");
    shader->m_shader_settings_.blend_src = shader_settings.GetInt("blend_src");
    shader->m_shader_settings_.blend_dst = shader_settings.GetInt("blend_dst");
    shader->m_shader_settings_.blend_op = shader_settings.GetInt("blend_op");
    shader->m_shader_settings_.color_mask = shader_settings.GetInt("color_mask");
    shader->m_shader_settings_.alpha_to_mask = shader_settings.GetInt("alpha_to_mask");

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

bool ShaderImporter::WriteShaderMeta(const std::shared_ptr<Shader> &shader, const PersistentDataStore data_store)
{
    std::stringstream string_buffer;
    {
        Serializer serializer;
        if (!serializer.Save(string_buffer, shader, false))
        {
            return false;
        }
    }

    data_store.SetString(kShaderMetaKey, string_buffer.str());
    data_store.SetInt(kShaderMetaVersionKey, 3);
    return true;
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

    // `shader_meta` will be reloaded on-the-fly if
    // 1. it didnt exist
    // 2. it was outdated
    auto shader_meta = ctx->DataStore().GetString(kShaderMetaKey);

    if (shader_meta.empty())
    {
        ctx->LogImportWarning("No shader meta data found! Generating!");

        const auto temp_shader_obj = Object::Instantiate<Shader>();
        WriteShaderMeta(temp_shader_obj, ctx->DataStore());
        Object::Destroy(temp_shader_obj);

        shader_meta = ctx->DataStore().GetString(kShaderMetaKey);
    }

    // please be aware that the variable `shader_meta_version` could be outdated later in this function on version upgrade handling
    const auto shader_meta_version = ctx->DataStore().GetInt(kShaderMetaVersionKey);
    if (shader_meta_version > kShaderMetaVersion)
    {
        ctx->LogImportWarning(
            std::format(
                "Shader meta data version ({}) is newer than importer version ({}). Possible data loss!",
                shader_meta_version,
                kShaderMetaVersion
            )
        );
    }

    if (shader_meta_version < kShaderMetaVersion)
    {
        ctx->LogImportWarning("Shader meta data is outdated! Will be upgraded after this import");

        if (shader_meta_version < 3)
        {
            const auto temp_shader_obj = Object::Instantiate<Shader>();
            if (!LoadOldParameters(temp_shader_obj, ctx))
            {
                ctx->LogImportError("Failed to load shader parameters!");
                return;
            }
            WriteShaderMeta(temp_shader_obj, ctx->DataStore());
            Object::Destroy(temp_shader_obj);

            shader_meta = ctx->DataStore().GetString(kShaderMetaKey);
        }
    }

    std::stringstream ss(shader_meta);
    Serializer serializer;
    const auto shader = serializer.Load<Shader>(ss);
    if (!shader)
    {
        ctx->LogImportError("Failed to deserialize shader object");
        return;
    }

    if (!CompileShader(shader, ctx->AssetPath()))
    {
        ctx->LogImportError("Failed to compile shader!");
        return;
    }

    if (!PSOManager::Register(shader, ctx->AssetPath().filename().string()))
    {
        ctx->LogImportError("Failed to register shader to PSOManager!");
        return;
    }

    ctx->SetMainObject(shader);
}

void ShaderImporter::OnExport(AssetDescriptor *ctx)
{
    const auto shader = std::dynamic_pointer_cast<Shader>(ctx->MainObject());
    if (shader == nullptr)
    {
        ctx->LogImportError("This object cannot be exported with ShaderExporter");
        return;
    }

    WriteShaderMeta(shader, ctx->DataStore());
}
}