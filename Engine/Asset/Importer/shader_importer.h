#pragma once
#include "asset_importer.h"
#include "Rendering/shader_parameter.h"

namespace engine
{
class Shader;

/// <summary>
/// .hlslファイルをコンパイルしてShaderとしてインポートするImporterです。パラメーター情報などはshader_metaとしてDataStoreに保存します。
/// </summary>
class ShaderImporter : public AssetImporter
{
    static constexpr auto kShaderMetaVersionKey = "shader_meta_version";
    static constexpr auto kShaderMetaKey = "shader_meta";

    /// <summary>
    /// 頂点・ピクセル・ジオメトリシェーダーのパラメーターを読み取り、重複を除いて結合します。
    /// </summary>
    /// <param name="shader">コンパイル済みのShader</param>
    static std::vector<ShaderParameter> ReadShaderParameters(const std::shared_ptr<Shader> &shader);
    /// <summary>
    /// 定数バッファのリフレクションから、変数の名前と型を読み取ります。
    /// </summary>
    /// <param name="cb_reflect">定数バッファのリフレクション</param>
    /// <param name="bind_desc">定数バッファのバインド情報</param>
    static ShaderParameter ReadConstantVariables(ID3D12ShaderReflectionConstantBuffer *cb_reflect, const D3D12_SHADER_INPUT_BIND_DESC &bind_desc);
    /// <summary>
    /// HLSLの型情報から、対応する定数バッファの変数の型を取得します。
    /// </summary>
    /// <param name="type_desc">HLSLの型情報</param>
    /// <returns>対応する型がない場合はkConstantBufferDataType_Unknown</returns>
    static kConstantBufferDataType GetConstantBufferDataType(const D3D12_SHADER_TYPE_DESC &type_desc);
    /// <summary>
    /// base_parametersに含まれていないパラメーターのみを追加します。
    /// </summary>
    /// <param name="base_parameters">追加先</param>
    /// <param name="src_parameters">追加元</param>
    static void EmplaceShaderParameters(std::vector<ShaderParameter> &base_parameters, const std::vector<ShaderParameter> &src_parameters);
    /// <summary>
    /// パラメーターを読み直してShaderに設定します。既存のパラメーターの表示名は引き継ぎます。
    /// </summary>
    /// <param name="shader">対象のShader</param>
    static void UpdateShaderParameters(const std::shared_ptr<Shader> &shader);
    /// <summary>
    /// 頂点(vrt)・ピクセル(pix)・ジオメトリ(geo)シェーダーをコンパイルします。ジオメトリシェーダーのエントリーポイントがない場合は省略します。
    /// </summary>
    /// <param name="shader">コンパイル結果の格納先</param>
    /// <param name="file_path">.hlslファイルのパス</param>
    /// <param name="error_msg">失敗時のエラーメッセージの出力先</param>
    /// <returns>コンパイルに成功した場合はtrue</returns>
    static bool CompileShader(const std::shared_ptr<Shader> &shader, const std::wstring &file_path, std::string &error_msg);
    /// <summary>
    /// Shaderをシリアライズし、shader_metaとしてDataStoreに保存します。
    /// </summary>
    /// <param name="shader">保存するShader</param>
    /// <param name="data_store">保存先</param>
    /// <returns>保存に成功した場合はtrue</returns>
    static bool WriteShaderMeta(const std::shared_ptr<Shader> &shader, PersistentDataStore data_store);
    /// <summary>
    /// エンジン側で設定する予約済みのBuffer名かどうかを判定します。
    /// </summary>
    /// <param name="buffer_name">判定するBuffer名</param>
    static bool IsReservedBufferName(std::string_view buffer_name);
    
public:
    static constexpr int kShaderMetaVersion = 4;

    /// <summary>
    /// コンパイル済みのShaderのリフレクションから、予約済みの名前を除いたパラメーターを読み取ります。
    /// </summary>
    /// <param name="shader_blob">コンパイル済みのShader</param>
    static std::vector<ShaderParameter> ReadShaderBlob(const ComPtr<ID3D10Blob> &shader_blob);

    /// <summary>
    /// 対応する拡張子(".hlsl")を返します。
    /// </summary>
    std::vector<std::string> SupportedExtensions() override;
    /// <summary>
    /// ObjectがShaderかどうかを判定します。
    /// </summary>
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    /// <summary>
    /// shader_metaからShaderを復元してコンパイルし、パラメーターを更新してMainObjectとして設定します。shader_metaがない、または壊れている場合は作り直します。
    /// </summary>
    void OnImport(AssetDescriptor *ctx) override;
    /// <summary>
    /// MainObjectのShaderをshader_metaとしてDataStoreに保存します。
    /// </summary>
    void OnExport(AssetDescriptor *ctx) override;
};
}