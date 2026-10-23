#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_shader_compiler.h"

#include <d3d12shader.h>
#include <directx-dxc/dxcapi.h>

#include "Rendering/CaboRenderingSystem/Core/log.h"

#pragma comment(lib, "dxcompiler.lib")

namespace CRS
{
namespace
{
ShaderVariableType ToVariableType(const D3D12_SHADER_TYPE_DESC &type_desc)
{
    if (type_desc.Class == D3D_SVC_SCALAR)
    {
        switch (type_desc.Type)
        {
        case D3D_SVT_INT:
            return ShaderVariableType::Int;
        case D3D_SVT_UINT:
            return ShaderVariableType::UInt;
        case D3D_SVT_FLOAT:
            return ShaderVariableType::Float;
        default:
            return ShaderVariableType::Unknown;
        }
    }

    if (type_desc.Type != D3D_SVT_FLOAT)
        return ShaderVariableType::Unknown;

    if (type_desc.Class == D3D_SVC_VECTOR)
    {
        switch (type_desc.Columns)
        {
        case 2:
            return ShaderVariableType::Float2;
        case 3:
            return ShaderVariableType::Float3;
        case 4:
            return ShaderVariableType::Float4;
        default:
            return ShaderVariableType::Unknown;
        }
    }

    if ((type_desc.Class == D3D_SVC_MATRIX_COLUMNS || type_desc.Class == D3D_SVC_MATRIX_ROWS) &&
        type_desc.Rows == 4 && type_desc.Columns == 4)
        return ShaderVariableType::Float4x4;

    return ShaderVariableType::Unknown;
}
}

D3D12ShaderCompiler::D3D12ShaderCompiler()
{
    DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_utils_));
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_compiler_));
    if (m_utils_)
        m_utils_->CreateDefaultIncludeHandler(&m_include_handler_);

    if (!IsValid())
        LogError("Failed to create DXC instance. Is dxcompiler.dll present?");
}

D3D12ShaderCompiler::~D3D12ShaderCompiler() = default;

bool D3D12ShaderCompiler::IsValid() const
{
    return m_utils_ != nullptr && m_compiler_ != nullptr;
}

bool D3D12ShaderCompiler::Compile(const ShaderSource &source, std::vector<uint8_t> &out_bytecode,
                                  ShaderReflection &out_reflection, std::string &error_message) const
{
    if (!IsValid())
    {
        error_message = "DXC is not available";
        return false;
    }

    ComPtr<IDxcBlobEncoding> source_blob;
    if (FAILED(m_utils_->LoadFile(source.file_path.c_str(), nullptr, &source_blob)))
    {
        error_message = "Failed to load shader file";
        return false;
    }

    const wchar_t *target = source.stage == ShaderStage::Vertex ? L"vs_6_0" : L"ps_6_0";

    std::vector<std::wstring> define_args;
    for (const auto &define : source.defines)
        define_args.emplace_back(L"-D" + define);

    std::vector args = {
        source.file_path.c_str(),
        L"-E", source.entry_point.c_str(),
        L"-T", target,
#if defined(DEBUG) || defined(_DEBUG)
        DXC_ARG_DEBUG, DXC_ARG_SKIP_OPTIMIZATIONS, L"-Qembed_debug",
#else
        DXC_ARG_OPTIMIZATION_LEVEL3,
#endif
    };
    for (const auto &define_arg : define_args)
        args.push_back(define_arg.c_str());

    const DxcBuffer buffer = {source_blob->GetBufferPointer(), source_blob->GetBufferSize(), DXC_CP_ACP};

    ComPtr<IDxcResult> result;
    const HRESULT hr = m_compiler_->Compile(&buffer, args.data(), static_cast<UINT32>(args.size()),
                                            m_include_handler_.Get(), IID_PPV_ARGS(&result));

    HRESULT status = E_FAIL;
    if (SUCCEEDED(hr))
        result->GetStatus(&status);

    if (FAILED(hr) || FAILED(status))
    {
        ComPtr<IDxcBlobUtf8> errors;
        if (result)
            result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);

        error_message = errors && errors->GetStringLength() > 0 ? errors->GetStringPointer() : "Unknown error";
        return false;
    }

    ComPtr<IDxcBlob> object;
    result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&object), nullptr);
    if (object == nullptr)
    {
        error_message = "Compiler returned no object";
        return false;
    }

    const auto begin = static_cast<const uint8_t *>(object->GetBufferPointer());
    out_bytecode.assign(begin, begin + object->GetBufferSize());

    return Reflect(out_bytecode, out_reflection);
}

bool D3D12ShaderCompiler::Reflect(const std::vector<uint8_t> &bytecode, ShaderReflection &out_reflection) const
{
    out_reflection = {};

    ComPtr<ID3D12ShaderReflection> reflection;
    const DxcBuffer buffer = {bytecode.data(), bytecode.size(), 0};
    if (FAILED(m_utils_->CreateReflection(&buffer, IID_PPV_ARGS(&reflection))))
    {
        LogError("Failed to create shader reflection");
        return false;
    }

    D3D12_SHADER_DESC shader_desc;
    reflection->GetDesc(&shader_desc);

    for (UINT i = 0; i < shader_desc.BoundResources; ++i)
    {
        D3D12_SHADER_INPUT_BIND_DESC bind_desc;
        reflection->GetResourceBindingDesc(i, &bind_desc);

        switch (bind_desc.Type)
        {
        case D3D_SIT_CBUFFER: {
            const auto cb_reflection = reflection->GetConstantBufferByName(bind_desc.Name);
            D3D12_SHADER_BUFFER_DESC cb_desc;
            cb_reflection->GetDesc(&cb_desc);

            ConstantBufferInfo info;
            info.name = bind_desc.Name;
            info.slot = bind_desc.BindPoint;
            info.space = bind_desc.Space;
            info.size = cb_desc.Size;

            for (UINT j = 0; j < cb_desc.Variables; ++j)
            {
                const auto variable = cb_reflection->GetVariableByIndex(j);
                D3D12_SHADER_VARIABLE_DESC variable_desc;
                variable->GetDesc(&variable_desc);
                D3D12_SHADER_TYPE_DESC type_desc;
                variable->GetType()->GetDesc(&type_desc);

                info.variables.push_back({
                    variable_desc.Name, variable_desc.StartOffset, variable_desc.Size, ToVariableType(type_desc)
                });
            }

            out_reflection.constant_buffers.push_back(std::move(info));
            break;
        }
        case D3D_SIT_TEXTURE:
            out_reflection.resources.push_back({
                bind_desc.Name, bind_desc.BindPoint, bind_desc.Space,
                bind_desc.Dimension == D3D_SRV_DIMENSION_TEXTURECUBE
                    ? ShaderResourceKind::TextureCube
                    : ShaderResourceKind::Texture
            });
            break;
        case D3D_SIT_STRUCTURED:
            out_reflection.resources.push_back({bind_desc.Name, bind_desc.BindPoint, bind_desc.Space,
                                                ShaderResourceKind::StructuredBuffer});
            break;
        case D3D_SIT_BYTEADDRESS:
            out_reflection.resources.push_back({bind_desc.Name, bind_desc.BindPoint, bind_desc.Space,
                                                ShaderResourceKind::ByteAddressBuffer});
            break;
        case D3D_SIT_UAV_RWTYPED:
            out_reflection.resources.push_back({bind_desc.Name, bind_desc.BindPoint, bind_desc.Space,
                                                ShaderResourceKind::ReadWriteTexture});
            break;
        case D3D_SIT_UAV_RWSTRUCTURED:
        case D3D_SIT_UAV_RWBYTEADDRESS:
            out_reflection.resources.push_back({bind_desc.Name, bind_desc.BindPoint, bind_desc.Space,
                                                ShaderResourceKind::ReadWriteBuffer});
            break;
        case D3D_SIT_SAMPLER:
            out_reflection.resources.push_back({bind_desc.Name, bind_desc.BindPoint, bind_desc.Space, ShaderResourceKind::Sampler});
            break;
        default:
            break;
        }
    }

    return true;
}
}
