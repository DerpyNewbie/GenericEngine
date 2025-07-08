#include "pch.h"
#include "ShaderImporter.h"
#include "logger.h"
#include "Rendering/shader.h"

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

std::vector<std::string> ShaderImporter::SupportedExtensions()
{
    return {".hlsl"};
}

std::shared_ptr<Object> ShaderImporter::Import(AssetDescriptor *asset)
{
    auto shader = Object::Instantiate<Shader>();
    if (asset->guid == xg::Guid("aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa"))
    {
        shader = Shader::GetDefault();
        return shader;
    }
        CompileShader(shader,asset->path_hint);
    
        return shader;
}
}