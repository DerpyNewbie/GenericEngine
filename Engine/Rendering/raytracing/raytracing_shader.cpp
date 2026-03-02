#include "pch.h"
#include "raytracing_shader.h"

#pragma comment(lib, "dxcompiler.lib")

namespace engine
{
RaytracingShader::RaytracingShader(const std::wstring &file_path)
{
    ComPtr<IDxcUtils> utils;
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));

    ComPtr<IDxcCompiler3> compiler;
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));

    ComPtr<IDxcIncludeHandler> include_handler;
    utils->CreateDefaultIncludeHandler(&include_handler);

    ComPtr<IDxcBlobEncoding> source_blob;

    auto hr = utils->LoadFile(file_path.c_str(), nullptr, &source_blob);
    if (FAILED(hr))
        Logger::Log("Failed to load hlsl file");

    LPCWSTR args[] = {
        file_path.c_str(),
        L"-T", L"lib_6_3",
        L"-Qstrip_reflect",
        L"-O3"
    };

    DxcBuffer source_buffer;
    source_buffer.Ptr = source_blob->GetBufferPointer();
    source_buffer.Size = source_blob->GetBufferSize();
    source_buffer.Encoding = DXC_CP_ACP;

    ComPtr<IDxcResult> result;
    compiler->Compile(
        &source_buffer,
        args, _countof(args),
        include_handler.Get(),
        IID_PPV_ARGS(&result)
    );

    ComPtr<IDxcBlobUtf8> errors;
    result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
    if (errors != nullptr && errors->GetStringLength() > 0)
    {
        OutputDebugStringA(errors->GetStringPointer());
    }

    result->GetStatus(&hr);
    if (FAILED(hr))
    {
        Logger::Log("Failed to compile shader");
    }

    result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&m_shader_blob_), nullptr);
}

CD3DX12_SHADER_BYTECODE RaytracingShader::GetByteCode() const
{
    return CD3DX12_SHADER_BYTECODE(
        m_shader_blob_->GetBufferPointer(),
        m_shader_blob_->GetBufferSize()
    );
}
}