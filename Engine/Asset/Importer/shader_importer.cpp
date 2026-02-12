#include "pch.h"
#include "shader_importer.h"
#include "logger.h"
#include "Rendering/shader.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"

#include <d3dcompiler.h>

#include "serializer.h"

namespace
{
constexpr std::array<std::string_view, 11> kEngineParameters = {"WorldMatrix", "ViewProjMatrix", "BoneMatrices", "SceneData", "ShadowCascadeSlices", "LightCount", "LightViewProj", "Lights", "ShadowMaps", "smp", "shadowSampler"};
}

namespace engine
{
std::vector<std::shared_ptr<ShaderParameter>> ShaderImporter::ParseShaderParameters(const std::shared_ptr<Shader> &shader)
{
    std::vector<std::shared_ptr<ShaderParameter>> shader_parameters;
    
    auto vs_blob = shader->m_vs_blob_;
    auto ps_blob = shader->m_ps_blob_;

    auto vs_params = ParseShaderBlob(vs_blob);
    const auto ps_params = ParseShaderBlob(ps_blob);

    shader_parameters.insert(shader_parameters.end(), vs_params.begin(), vs_params.end());

    for (auto ps_param : ps_params)
    {
        auto same_param = std::ranges::find_if(shader_parameters, [&ps_param](const auto &shader_param) {
            return ps_param->display_name == shader_param->display_name;
        });
        if (same_param != shader_parameters.end())
            continue;

        shader_parameters.emplace_back(ps_param);
    }

    return shader_parameters;
}

std::vector<std::shared_ptr<ShaderParameter>> ShaderImporter::ParseShaderBlob(const ComPtr<ID3D10Blob> &shader_blob)
{
    std::vector<std::shared_ptr<ShaderParameter>> shader_parameters;

    ComPtr<ID3D12ShaderReflection> reflector;
    D3DReflect(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), IID_PPV_ARGS(&reflector));

    D3D12_SHADER_DESC shader_desc;
    reflector->GetDesc(&shader_desc);

    for (UINT i = 0; i < shader_desc.ConstantBuffers; ++i)
    {
        ID3D12ShaderReflectionConstantBuffer *buffer_reflector = reflector->GetConstantBufferByIndex(i);

        D3D12_SHADER_BUFFER_DESC buffer_desc;
        buffer_reflector->GetDesc(&buffer_desc);

        auto is_engine_parameter = std::ranges::find(kEngineParameters, buffer_desc.Name) != kEngineParameters.end();
        if (is_engine_parameter)
            continue;

        auto buffer_parameters = ParseConstantBufferShaderVariables(buffer_reflector, reflector);
        shader_parameters.insert(shader_parameters.end(), buffer_parameters.begin(), buffer_parameters.end());
    }

    for (UINT i = 0; i < shader_desc.BoundResources; ++i)
    {
        D3D12_SHADER_INPUT_BIND_DESC bind_desc;
        reflector->GetResourceBindingDesc(i, &bind_desc);

        if (bind_desc.Type == D3D_SIT_TEXTURE)
        {
            auto is_engine_parameter = std::ranges::find(kEngineParameters, bind_desc.Name) != kEngineParameters.end();
            if (is_engine_parameter)
                continue;

            auto buffer_parameters = ParseTextureBufferShaderVariable(&bind_desc, reflector);
            shader_parameters.emplace_back(buffer_parameters);
        }
    }
    return shader_parameters;
}

std::vector<std::shared_ptr<ShaderParameter>> ShaderImporter::ParseConstantBufferShaderVariables(ID3D12ShaderReflectionConstantBuffer *reflection_buffer, const ComPtr<ID3D12ShaderReflection> &reflector)
{
    std::vector<std::shared_ptr<ShaderParameter>> variables;
    D3D12_SHADER_BUFFER_DESC shader_desc;
    reflection_buffer->GetDesc(&shader_desc);

    D3D12_SHADER_INPUT_BIND_DESC bind_desc;
    reflector->GetResourceBindingDescByName(shader_desc.Name, &bind_desc);
    for (auto i = 0; i < shader_desc.Variables; ++i)
    {
        variables.emplace_back(ParseTextureBufferShaderVariable(&bind_desc, reflector));
    }

    return variables;
}

std::shared_ptr<ShaderParameter> ShaderImporter::ParseTextureBufferShaderVariable(D3D12_SHADER_INPUT_BIND_DESC *bind_desc, const ComPtr<ID3D12ShaderReflection> &reflector)
{
    std::shared_ptr<ShaderParameter> shader_parameter;

    return ParseShaderVariable(bind_desc->BindPoint, bind_desc);
}

std::shared_ptr<ShaderParameter> ShaderImporter::ParseShaderVariable(int register_idx, ID3D12ShaderReflectionVariable *variable)
{
    D3D12_SHADER_VARIABLE_DESC desc;
    variable->GetDesc(&desc);
    return std::make_shared<ShaderParameter>(register_idx, desc.Name, desc.Name, GetTypeHint(variable));
}

std::shared_ptr<ShaderParameter> ShaderImporter::ParseShaderVariable(int register_idx, D3D12_SHADER_INPUT_BIND_DESC *bind_desc)
{
    return std::make_shared<ShaderParameter>(register_idx, bind_desc->Name, bind_desc->Name, GetTypeHint(bind_desc));
}

std::string ShaderImporter::GetTypeHint(ID3D12ShaderReflectionVariable *variable)
{
    auto type = variable->GetType();

    D3D12_SHADER_TYPE_DESC type_desc;
    type->GetDesc(&type_desc);

    switch (type_desc.Type)
    {
        case D3D_SVT_INT:
            switch (type_desc.Class)
            {
                case D3D_SVC_SCALAR:
                    return "int";
                default:
                    return "unknown";
            }
        case D3D_SVT_FLOAT:
            switch (type_desc.Class)
            {
                case D3D_SVC_SCALAR:
                    return "float";
                case D3D_SVC_VECTOR:
                    switch (type_desc.Columns)
                    {
                        case 2:
                            return "float2";
                        case 3:
                            return "float3";
                        case 4:
                            return "color";
                        default:
                            return "unknown";
                    }
                default:
                    return "unknown";
            }
        default:
            return "unknown";
    }
}

std::string ShaderImporter::GetTypeHint(const D3D12_SHADER_INPUT_BIND_DESC *bind_desc)
{
    switch (bind_desc->Type)
    {
        case D3D_SIT_TEXTURE:
            switch (bind_desc->Dimension)
            {
                case D3D_SRV_DIMENSION_TEXTURE2D:
                    return "texture2d";
                default:
                    return "unknown";
            }
        default:
            return "unknown";
    }
}

void ShaderImporter::CreateShaderParameters(const std::shared_ptr<Shader> &shader)
{
    shader->parameters = ParseShaderParameters(shader);
}

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
        &shader->m_ps_blob_,
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
    auto construct_shader_parameter = [&](const int index, auto &object) {
        auto param = std::make_shared<ShaderParameter>();
        param->index = index;

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

    auto read_shader_params = [&](const char *name) {
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
            params.emplace_back(construct_shader_parameter(index, i));
        }

        return params;
    };

    auto vertex_params = read_shader_params("vertex");
    auto pixel_params = read_shader_params("pixel");

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

    /*if (shader_meta_version < kShaderMetaVersion)
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
    }*/

    std::stringstream ss(shader_meta);
    Serializer serializer;
    const auto shader = serializer.Load<Shader>(ss);
    /*if (!shader)
    {
        ctx->LogImportError("Failed to deserialize shader object");
        return;
    }*/

    if (!CompileShader(shader, ctx->AssetPath()))
    {
        ctx->LogImportError("Failed to compile shader!");
        return;
    }

    if (shader->parameters.empty())
    {
        CreateShaderParameters(shader);
        WriteShaderMeta(shader, ctx->DataStore());

        shader_meta = ctx->DataStore().GetString(kShaderMetaKey);
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