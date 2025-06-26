#include "pch.h"

#include "shader.h"

#include "logger.h"

bool engine::PixelShader::CompileShader(std::wstring file_path)
{
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompileFromFile(
        file_path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_5_0",
        0,
        0,
        &m_pShaderBlob,
        &errorBlob
        );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            Logger::Error<PixelShader>("Failed to Compile Pixel Shader!");
            return false;
        }
    }
    return true;
}

bool engine::VertexShader::CompileShader(std::wstring file_path)
{
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompileFromFile(
        file_path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_5_0",
        0,
        0,
        &m_pShaderBlob,
        &errorBlob
        );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            Logger::Error<PixelShader>("Failed to Compile Vertex Shader!");
            return false;
        }
    }
    return true;
}