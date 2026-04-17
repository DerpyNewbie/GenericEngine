#include "pch.h"
#include "shader_importer.h"
#include "logger.h"
#include "Rendering/shader.h"
#include "Rendering/CabotEngine/Graphics/PSOManager.h"

#include <d3dcompiler.h>

#include "serializer.h"

namespace
{
constexpr std::array<std::string_view, 11> kReservedBufferNames = {"WorldMatrix", "ViewProjMatrix", "BoneMatrices", "SceneData", "ShadowCascadeSlices", "LightCount", "LightViewProj", "Lights", "ShadowMaps", "smp", "shadowSampler"};
}

namespace engine
{
std::vector<ShaderParameter> ShaderImporter::ReadShaderParameters(const std::shared_ptr<Shader> &shader)
{
    const auto vs_blob = shader->m_vs_blob_;
    const auto ps_blob = shader->m_ps_blob_;

    const auto vs_params = ReadShaderBlob(vs_blob);
    const auto ps_params = ReadShaderBlob(ps_blob);

    std::vector<ShaderParameter> shader_parameters;
    shader_parameters.reserve(vs_params.size() + ps_params.size());
    shader_parameters.insert(shader_parameters.end(), vs_params.begin(), vs_params.end());

    for (auto ps_param : ps_params)
    {
        auto same_param = std::ranges::find(shader_parameters, ps_param) != shader_parameters.end();
        if (same_param)
            continue;

        shader_parameters.emplace_back(ps_param);
    }

    shader_parameters.shrink_to_fit();
    
    return shader_parameters;
}

std::vector<ShaderParameter> ShaderImporter::ReadShaderBlob(const ComPtr<ID3D10Blob> &shader_blob)
{
    std::vector<ShaderParameter> shader_parameters;

    ComPtr<ID3D12ShaderReflection> shader;
    D3DReflect(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), IID_PPV_ARGS(&shader));

    D3D12_SHADER_DESC shader_desc;
    shader->GetDesc(&shader_desc);

    for (UINT i = 0; i < shader_desc.ConstantBuffers; ++i)
    {
        ID3D12ShaderReflectionConstantBuffer *constant_buffer = shader->GetConstantBufferByIndex(i);

        D3D12_SHADER_BUFFER_DESC buffer_desc;
        constant_buffer->GetDesc(&buffer_desc);

        if (IsReservedBufferName(buffer_desc.Name))
            continue;

        auto cb_parameters = ReadConstantBufferVariables(shader, constant_buffer);
        shader_parameters.insert(shader_parameters.end(), cb_parameters.begin(), cb_parameters.end());
    }

    for (UINT i = 0; i < shader_desc.BoundResources; ++i)
    {
        D3D12_SHADER_INPUT_BIND_DESC bind_desc;
        shader->GetResourceBindingDesc(i, &bind_desc);

        if (IsReservedBufferName(bind_desc.Name))
            continue;

        auto buffer_parameters = ConvertToShaderParameter(&bind_desc);
        shader_parameters.emplace_back(buffer_parameters);
    }
    return shader_parameters;
}

std::vector<ShaderParameter> ShaderImporter::ReadConstantBufferVariables(const ComPtr<ID3D12ShaderReflection> &shader, ID3D12ShaderReflectionConstantBuffer *constant_buffer)
{
    std::vector<ShaderParameter> result;

    D3D12_SHADER_BUFFER_DESC buffer_desc;
    constant_buffer->GetDesc(&buffer_desc);

    D3D12_SHADER_INPUT_BIND_DESC bind_desc;
    shader->GetResourceBindingDescByName(buffer_desc.Name, &bind_desc);

    for (auto i = 0; i < buffer_desc.Variables; ++i)
    {
        const auto variable = constant_buffer->GetVariableByIndex(i);
        D3D12_SHADER_VARIABLE_DESC variable_desc;
        variable->GetDesc(&variable_desc);

        const auto variable_type = variable->GetType();
        D3D12_SHADER_TYPE_DESC type_desc;
        variable_type->GetDesc(&type_desc);

        result.emplace_back(ConvertToShaderParameter(bind_desc.BindPoint, variable_desc, type_desc));
    }

    return result;
}

ShaderParameter ShaderImporter::ConvertToShaderParameter(const D3D12_SHADER_INPUT_BIND_DESC *bind_desc)
{
    return ShaderParameter{static_cast<int>(bind_desc->BindPoint), bind_desc->Name, bind_desc->Name, GetTypeHint(bind_desc)};
}

ShaderParameter ShaderImporter::ConvertToShaderParameter(const UINT register_idx, const D3D12_SHADER_VARIABLE_DESC &variable_desc, const D3D12_SHADER_TYPE_DESC &type_desc)
{
    return ShaderParameter{static_cast<int>(register_idx), variable_desc.Name, variable_desc.Name, GetTypeHint(type_desc)};
}

std::string ShaderImporter::GetTypeHint(const D3D12_SHADER_TYPE_DESC &type_desc)
{
    switch (type_desc.Type)
    {
        case D3D_SVT_INT:
            switch (type_desc.Class)
            {
                case D3D_SVC_SCALAR:
                    return "int";
                default:
                    return std::format("type_desc_not_supported_int_class_{}", static_cast<int>(type_desc.Class));
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
                            return std::format("type_desc_not_supported_float_vector_col_{}", static_cast<int>(type_desc.Columns));
                    }
                default:
                    return std::format("type_desc_not_supported_float_class_{}", static_cast<int>(type_desc.Class));
            }
        default:
            return std::format("type_desc_not_supported_type_{}", static_cast<int>(type_desc.Type));
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
                    return std::format("input_bind_not_supported_texture_dimension_{}", static_cast<int>(bind_desc->Dimension));
            }
        default:
            return std::format("input_bind_not_supported_type_{}", static_cast<int>(bind_desc->Type));
    }
}

void ShaderImporter::UpdateShaderParameters(const std::shared_ptr<Shader> &shader)
{
    auto old_parameters = shader->parameters;
    auto parameters = ReadShaderParameters(shader);
    for (auto &parameter : parameters)
    {
        auto pos = std::ranges::find(old_parameters, parameter);
        // its a new parameter
        if (pos == old_parameters.end())
            continue;

        parameter.display_name = pos->display_name;
    }

    shader->parameters = parameters;
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
        if (error_blob && error_blob->GetBufferPointer() && error_blob->GetBufferSize() > 0)
        {
            const auto error_msg = static_cast<const char *>(error_blob->GetBufferPointer());
            OutputDebugStringA(error_msg);
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
        Logger::Error<ShaderImporter>("Failed to Compile Pixel Shader!");
        if (error_blob && error_blob->GetBufferPointer() && error_blob->GetBufferSize() > 0)
        {
            const auto error_msg = static_cast<const char *>(error_blob->GetBufferPointer());
            OutputDebugStringA(error_msg);
        }
        return false;
    }

    // Add shader variants here if you want to
    return true;
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

bool ShaderImporter::IsReservedBufferName(const std::string_view buffer_name)
{
    return std::ranges::find(kReservedBufferNames, buffer_name) != kReservedBufferNames.end();
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

    std::stringstream ss(shader_meta);
    Serializer serializer;
    auto shader = serializer.Load<Shader>(ss);
    if (shader == nullptr)
    {
        ctx->LogImportWarning("Corrupted shader meta data found! Regenerating!");

        shader = Object::Instantiate<Shader>();
        WriteShaderMeta(shader, ctx->DataStore());

        shader_meta = ctx->DataStore().GetString(kShaderMetaKey);
    }

    if (!CompileShader(shader, ctx->AssetPath()))
    {
        ctx->LogImportError("Failed to compile shader!");
        return;
    }

    UpdateShaderParameters(shader);
    WriteShaderMeta(shader, ctx->DataStore());

    shader_meta = ctx->DataStore().GetString(kShaderMetaKey);

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