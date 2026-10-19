#pragma once
#include "asset_importer.h"

namespace engine
{
/// <summary>
/// MaterialのImport / Exportを行うImporterです。
/// </summary>
class MaterialImporter : public AssetImporter
{
public:
    /// <summary>
    /// 対応する拡張子(".material")を返します。
    /// </summary>
    std::vector<std::string> SupportedExtensions() override;
    /// <summary>
    /// ObjectがMaterialであるかどうかを判定します。
    /// </summary>
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    /// <summary>
    /// ファイルからMaterialをDeserializeし、MainObjectとして設定します。
    /// </summary>
    void OnImport(AssetDescriptor *ctx) override;
    /// <summary>
    /// MainObjectのMaterialをSerializeしてファイルに書き出します。
    /// </summary>
    void OnExport(AssetDescriptor *ctx) override;
};
}