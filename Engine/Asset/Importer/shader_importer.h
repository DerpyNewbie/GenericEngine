#pragma once
#include "asset_importer.h"
#include "Rendering/shader_parameter.h"

namespace engine
{
class Shader;

/// <summary>
/// ShaderファイルをCompileし、ShaderParameterを読み取ってImportするImporterです。
/// </summary>
class ShaderImporter : public AssetImporter
{
    static constexpr auto kShaderMetaVersionKey = "shader_meta_version";
    static constexpr auto kShaderMetaKey = "shader_meta";

    /// <summary>
    /// VertexShader、PixelShader、GeometryShaderからShaderParameterを読み取り、重複を除いてまとめます。
    /// </summary>
    static std::vector<ShaderParameter> ReadShaderParameters(const std::shared_ptr<Shader> &shader);
    /// <summary>
    /// コンパイル済みのShaderから、ConstantBufferの変数とbindされているResourceをShaderParameterとして読み取ります。Engineが予約している名前は除かれます。
    /// </summary>
    static std::vector<ShaderParameter> ReadShaderBlob(const ComPtr<ID3D10Blob> &shader_blob);
    /// <summary>
    /// ConstantBufferに含まれる変数をShaderParameterとして読み取ります。
    /// </summary>
    static std::vector<ShaderParameter> ReadConstantBufferVariables(const ComPtr<ID3D12ShaderReflection> &shader, ID3D12ShaderReflectionConstantBuffer *constant_buffer);
    /// <summary>
    /// bindされているResourceの情報をShaderParameterに変換します。
    /// </summary>
    static ShaderParameter ConvertToShaderParameter(const D3D12_SHADER_INPUT_BIND_DESC *bind_desc);
    /// <summary>
    /// ConstantBufferの変数の情報をShaderParameterに変換します。
    /// </summary>
    /// <param name="register_idx">ConstantBufferのregister番号</param>
    static ShaderParameter ConvertToShaderParameter(UINT register_idx, const D3D12_SHADER_VARIABLE_DESC &variable_desc, const D3D12_SHADER_TYPE_DESC &type_desc);
    /// <summary>
    /// 変数の型を表す文字列("int"、"float"、"float2"、"float3"、"color")を取得します。
    /// </summary>
    static std::string GetTypeHint(const D3D12_SHADER_TYPE_DESC &type_desc);
    /// <summary>
    /// Resourceの型を表す文字列("texture2d")を取得します。
    /// </summary>
    static std::string GetTypeHint(const D3D12_SHADER_INPUT_BIND_DESC *bind_desc);
    /// <summary>
    /// src_parametersのうち、base_parametersに含まれていないものだけを追加します。
    /// </summary>
    static void EmplaceShaderParameters(std::vector<ShaderParameter> &base_parameters, const std::vector<ShaderParameter> &src_parameters);
    /// <summary>
    /// Shaderのparametersを読み取り直します。既にあるparameterのdisplay_nameは引き継がれます。
    /// </summary>
    static void UpdateShaderParameters(const std::shared_ptr<Shader> &shader);
    /// <summary>
    /// ファイルからVertexShader("vrt")、PixelShader("pix")、GeometryShader("geo")をコンパイルします。
    /// </summary>
    /// <returns>VertexShaderのコンパイルに失敗した場合 false</returns>
    static bool CompileShader(const std::shared_ptr<Shader> &shader, const std::wstring &file_path);
    /// <summary>
    /// ShaderをSerializeしてDataStoreに保存します。
    /// </summary>
    /// <returns>Serializeに失敗した場合 false</returns>
    static bool WriteShaderMeta(const std::shared_ptr<Shader> &shader, PersistentDataStore data_store);
    /// <summary>
    /// Engineが使用する予約済みのBuffer名であるかどうかを判定します。
    /// </summary>
    static bool IsReservedBufferName(std::string_view buffer_name);

public:
    static constexpr int kShaderMetaVersion = 4;

    /// <summary>
    /// 対応する拡張子(".hlsl")を返します。
    /// </summary>
    std::vector<std::string> SupportedExtensions() override;
    /// <summary>
    /// ObjectがShaderであるかどうかを判定します。
    /// </summary>
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    /// <summary>
    /// ".meta"に保存されたデータからShaderを復元し、コンパイルしてparametersを更新します。
    /// </summary>
    void OnImport(AssetDescriptor *ctx) override;
    /// <summary>
    /// MainObjectのShaderをSerializeして".meta"のDataStoreに保存します。
    /// </summary>
    void OnExport(AssetDescriptor *ctx) override;
};
}