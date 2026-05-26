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
    const auto gs_blob = shader->m_gs_blob_;

    const auto vs_params = ReadShaderBlob(vs_blob);
    const auto ps_params = ReadShaderBlob(ps_blob);

    std::vector<ShaderParameter> shader_parameters;
    shader_parameters.reserve(vs_params.size() + ps_params.size());
    shader_parameters.insert(shader_parameters.end(), vs_params.begin(), vs_params.end());

    EmplaceShaderParameters(shader_parameters, ps_params);

    if (gs_blob != nullptr)
    {
        const auto gs_params = ReadShaderBlob(gs_blob);
        EmplaceShaderParameters(shader_parameters, gs_params);
    }

    shader_parameters.shrink_to_fit();

    return shader_parameters;
}

std::vector<ShaderParameter> ShaderImporter::ReadShaderBlob(const ComPtr<ID3D10Blob> &shader_blob)
{
    std::vector<ShaderParameter> shader_parameters;

    ComPtr<ID3D12ShaderReflection> shader_reflect;
    D3DReflect(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), IID_PPV_ARGS(&shader_reflect));

    D3D12_SHADER_DESC shader_desc;
    shader_reflect->GetDesc(&shader_desc);

    for (UINT i = 0; i < shader_desc.BoundResources; ++i)
    {
        D3D12_SHADER_INPUT_BIND_DESC bind_desc;
        shader_reflect->GetResourceBindingDesc(i, &bind_desc);

        if (IsReservedBufferName(bind_desc.Name))
            continue;

        switch (bind_desc.Type)
        {
            case D3D_SIT_CBUFFER: {
                ID3D12ShaderReflectionConstantBuffer *cb_reflect = shader_reflect->GetConstantBufferByName(bind_desc.Name);
                shader_parameters.emplace_back(ReadConstantVariables(cb_reflect, bind_desc));
                break;
            }

            case D3D_SIT_STRUCTURED: {
                ShaderParameter shader_param = {static_cast<int>(bind_desc.BindPoint), bind_desc.Name, bind_desc.Name, kBufferType_StructuredBuffer};
                shader_parameters.emplace_back(shader_param);
                break;
            }

            case D3D_SIT_UAV_RWSTRUCTURED: {
                ShaderParameter shader_param = {static_cast<int>(bind_desc.BindPoint), bind_desc.Name, bind_desc.Name, kBufferType_StructuredBuffer, true};
                shader_parameters.emplace_back(shader_param);
                break;
            }

            case D3D_SIT_TEXTURE: {
                ShaderParameter shader_param = {static_cast<int>(bind_desc.BindPoint), bind_desc.Name, bind_desc.Name, kBufferType_Texture2D};
                shader_parameters.emplace_back(shader_param);
                break;
            }

            case D3D_SIT_UAV_RWTYPED: {
                ShaderParameter shader_param = {static_cast<int>(bind_desc.BindPoint), bind_desc.Name, bind_desc.Name, kBufferType_UavTexture, true};
                shader_parameters.emplace_back(shader_param);
                break;
            }
        }
    }

    return shader_parameters;
}

ShaderParameter ShaderImporter::ReadConstantVariables(ID3D12ShaderReflectionConstantBuffer *cb_reflect, const D3D12_SHADER_INPUT_BIND_DESC &bind_desc)
{
    D3D12_SHADER_BUFFER_DESC cb_desc;
    cb_reflect->GetDesc(&cb_desc);

    ShaderParameter shader_param = {static_cast<int>(bind_desc.BindPoint), bind_desc.Name, bind_desc.Name, kBufferType_ConstantBuffer, false, cb_desc.Size};

    for (UINT j = 0; j < cb_desc.Variables; ++j)
    {
        const auto var_reflect = cb_reflect->GetVariableByIndex(j);
        const auto type_reflect = var_reflect->GetType();

        D3D12_SHADER_VARIABLE_DESC var_desc;
        var_reflect->GetDesc(&var_desc);

        D3D12_SHADER_TYPE_DESC type_desc;
        type_reflect->GetDesc(&type_desc);

        ShaderVariableDesc variable_info;
        variable_info.name = var_desc.Name;
        variable_info.data_type = GetConstantBufferDataType(type_desc);

        shader_param.variables.emplace_back(variable_info);
    }

    return shader_param;
}

kConstantBufferDataType ShaderImporter::GetConstantBufferDataType(const D3D12_SHADER_TYPE_DESC &type_desc)
{
    switch (type_desc.Type)
    {
        case D3D_SVT_INT:
            switch (type_desc.Class)
            {
                case D3D_SVC_SCALAR:
                    return kConstantBufferDataType::kConstantBufferDataType_Int;
                default:
                    return kConstantBufferDataType::kConstantBufferDataType_Unknown;
            }
        case D3D_SVT_FLOAT:
            switch (type_desc.Class)
            {
                case D3D_SVC_SCALAR:
                    return kConstantBufferDataType::kConstantBufferDataType_Float;
                case D3D_SVC_VECTOR:
                    switch (type_desc.Columns)
                    {
                        case 2:
                            return kConstantBufferDataType::kConstantBufferDataType_Vector2;
                        case 3:
                            return kConstantBufferDataType::kConstantBufferDataType_Vector3;
                        case 4:
                            return kConstantBufferDataType::kConstantBufferDataType_Color;
                        default:
                            return kConstantBufferDataType::kConstantBufferDataType_Unknown;
                    }
                case D3D_SVC_MATRIX_COLUMNS:
                    if (type_desc.Rows == 4 && type_desc.Columns == 4)
                        return kConstantBufferDataType::kConstantBufferDataType_Matrix;
            }
        default:
            return kConstantBufferDataType::kConstantBufferDataType_Unknown;
    }
}

void ShaderImporter::EmplaceShaderParameters(std::vector<ShaderParameter> &base_parameters, const std::vector<ShaderParameter> &src_parameters)
{
    for (auto src_param : src_parameters)
    {
        if (std::ranges::find(base_parameters, src_param) == base_parameters.end())
            base_parameters.emplace_back(src_param);
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
        if (error_blob != nullptr)
        {
            // 1. error_blob からエラー文字列の先頭ポインタを取得
            auto error_message = static_cast<const char *>(error_blob->GetBufferPointer());

            // 2. Visual Studio の「出力」ウインドウに表示する場合（超おすすめ）
            OutputDebugStringA("\n================ HLSL Compile Error ================\n");
            OutputDebugStringA(error_message);
            OutputDebugStringA("====================================================\n\n");
        }
    }
    
    hr = D3DCompileFromFile(
        file_path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "geo",
        "gs_5_0",
        0,
        0,
        &shader->m_gs_blob_,
        &error_blob
    );

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