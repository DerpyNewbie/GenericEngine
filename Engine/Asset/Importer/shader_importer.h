#pragma once
#include "asset_importer.h"

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
    /// Shaderをシリアライズし、shader_metaとしてDataStoreに保存します。
    /// </summary>
    /// <param name="shader">保存するShader</param>
    /// <param name="data_store">保存先</param>
    /// <returns>保存に成功した場合はtrue</returns>
    static bool WriteShaderMeta(const std::shared_ptr<Shader> &shader, PersistentDataStore data_store);

public:
    static constexpr int kShaderMetaVersion = 5;

    /// <summary>
    /// 対応する拡張子(".hlsl")を返します。
    /// </summary>
    std::vector<std::string> SupportedExtensions() override;
    /// <summary>
    /// ObjectがShaderかどうかを判定します。
    /// </summary>
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    /// <summary>
    /// shader_metaからShaderを復元してコンパイルし、MainObjectとして設定します。shader_metaがない、または壊れている場合は作り直します。
    /// </summary>
    void OnImport(AssetDescriptor *ctx) override;
    /// <summary>
    /// MainObjectのShaderをshader_metaとしてDataStoreに保存します。
    /// </summary>
    void OnExport(AssetDescriptor *ctx) override;
};
}
