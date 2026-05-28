#include "pch.h"
#include "compute_shader_importer.h"

#include "shader_importer.h"
#include "Rendering/compute_shader.h"
#include "Rendering/shader.h"

namespace engine
{
bool ComputeShaderImporter::CompileShader(const std::shared_ptr<ComputeShader> &compute_shader, const std::wstring &file_path, std::string &error_msg)
{
    ComPtr<ID3DBlob> error_blob;

    HRESULT hr = D3DCompileFromFile(
        file_path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "compute",
        "cs_5_0",
        D3DCOMPILE_SKIP_OPTIMIZATION,
        D3DCOMPILE_DEBUG,
        &compute_shader->m_shader_blob_,
        &error_blob
    );

    if (FAILED(hr))
    {
        Logger::Error<ComputeShaderImporter>("Failed to Compile Compute Shader!");
        if (error_blob && error_blob->GetBufferPointer() && error_blob->GetBufferSize() > 0)
        {
            error_msg = static_cast<const char *>(error_blob->GetBufferPointer());
        }
        else
        {
            error_msg = "NULL";
        }

        return false;
    }

    return true;
}

std::vector<std::string> ComputeShaderImporter::SupportedExtensions()
{
    return {".compute"};
}

bool ComputeShaderImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<ComputeShader>(object) != nullptr;
}

void ComputeShaderImporter::OnImport(AssetDescriptor *ctx)
{
    const auto shader = Object::Instantiate<ComputeShader>();

    std::string error_msg;
    if (!CompileShader(shader, ctx->AssetPath(), error_msg))
    {
        ctx->LogImportError(error_msg);
        return;
    }

    shader->parameters = ShaderImporter::ReadShaderBlob(shader->m_shader_blob_);
    ctx->SetMainObject(shader);
}
}