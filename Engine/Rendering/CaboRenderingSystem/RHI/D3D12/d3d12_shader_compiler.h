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
    /// <summary>
    /// DXCを読み込みます（dxcompiler.dll / dxil.dll が実行ファイルの横に必要です）。失敗した場合はIsValid()がfalseになります。
    /// </summary>
    D3D12ShaderCompiler();
    ~D3D12ShaderCompiler();

    /// <summary>
    /// DXCを読み込めたかどうかです。
    /// </summary>
    bool IsValid() const;

    /// <summary>
    /// シェーダーモデル6でコンパイルし、リフレクションも取り出します。#includeはソースのファイルからの相対パスで探します。
    /// 失敗した場合はfalseを返し、error_messageにエラーを入れます。
    /// </summary>
    bool Compile(const ShaderSource &source, std::vector<uint8_t> &out_bytecode, ShaderReflection &out_reflection,
                 std::string &error_message) const;

    /// <summary>
    /// コンパイル済みのDXILからリフレクションを取り出します。
    /// </summary>
    bool Reflect(const std::vector<uint8_t> &bytecode, ShaderReflection &out_reflection) const;
};
}
