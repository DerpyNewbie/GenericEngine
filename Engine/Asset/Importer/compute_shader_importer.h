#pragma once
#include "asset_importer.h"

namespace engine
{
class ComputeShader;

/// <summary>
/// .computeファイルをコンパイルしてComputeShaderとしてインポートするImporterです。
/// </summary>
class ComputeShaderImporter : public AssetImporter
{
    static constexpr auto kComputeShaderMetaKey = "compute_shader_meta";

    /// <summary>
    /// ComputeShaderをシリアライズし、DataStoreに保存します。
    /// </summary>
    /// <returns>保存に成功した場合はtrue</returns>
    static bool WriteMeta(const std::shared_ptr<ComputeShader> &shader, PersistentDataStore data_store);

public:
    /// <summary>
    /// 対応する拡張子(".compute")を返します。
    /// </summary>
    std::vector<std::string> SupportedExtensions() override;
    /// <summary>
    /// ObjectがComputeShaderかどうかを判定します。
    /// </summary>
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    /// <summary>
    /// metaからComputeShaderを復元してコンパイルし、MainObjectとして設定します。metaがない、または壊れている場合は作り直します。
    /// </summary>
    void OnImport(AssetDescriptor *ctx) override;
    /// <summary>
    /// MainObjectのComputeShaderをDataStoreに保存します。
    /// </summary>
    void OnExport(AssetDescriptor *ctx) override;
};
}