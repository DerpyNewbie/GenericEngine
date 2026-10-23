#pragma once
#include <string>
#include <vector>

#include "Rendering/CaboRenderingSystem/RHI/shader_reflection.h"
#include "Rendering/CaboRenderingSystem/RHI/D3D12/d3d12_common.h"

struct IDxcUtils;
struct IDxcCompiler3;
struct IDxcIncludeHandler;

namespace CRS
{
/// <summary>
/// DXCでHLSLをDXILにコンパイルし、リフレクションを自前の型に詰め替えます。
/// </summary>
class D3D12ShaderCompiler
{
    ComPtr<IDxcUtils> m_utils_;
    ComPtr<IDxcCompiler3> m_compiler_;
    ComPtr<IDxcIncludeHandler> m_include_handler_;

public:
    D3D12ShaderCompiler();
    ~D3D12ShaderCompiler();

    bool IsValid() const;

    /// <summary>
    /// コンパイルします。失敗した場合はfalseを返し、error_messageにエラーを入れます。
    /// </summary>
    bool Compile(const ShaderSource &source, std::vector<uint8_t> &out_bytecode, ShaderReflection &out_reflection,
                 std::string &error_message) const;

    /// <summary>
    /// コンパイル済みのDXILからリフレクションを取り出します。
    /// </summary>
    bool Reflect(const std::vector<uint8_t> &bytecode, ShaderReflection &out_reflection) const;
};
}
