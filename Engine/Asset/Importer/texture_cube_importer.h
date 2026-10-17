#pragma once
#include "asset_importer.h"

namespace engine
{
/// <summary>
/// TextureCubeのImport / Exportを行うImporterです。
/// </summary>
class TextureCubeImporter : public AssetImporter
{
public:
    /// <summary>
    /// 対応する拡張子(".cubemap")を返します。
    /// </summary>
    std::vector<std::string> SupportedExtensions() override;
    /// <summary>
    /// ObjectがTextureCubeであるかどうかを判定します。
    /// </summary>
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    /// <summary>
    /// ファイルからTextureCubeをDeserializeし、MainObjectとして設定します。
    /// </summary>
    void OnImport(AssetDescriptor *ctx) override;
    /// <summary>
    /// MainObjectのTextureCubeをSerializeしてファイルに書き出します。
    /// </summary>
    void OnExport(AssetDescriptor *ctx) override;
};
}