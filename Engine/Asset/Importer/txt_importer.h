#pragma once
#include "Asset/Importer/asset_importer.h"

namespace engine
{
/// <summary>
/// テキストファイルをTextAssetとしてImport / ExportするImporterです。
/// </summary>
class TxtImporter : public AssetImporter
{
public:
    /// <summary>
    /// 対応する拡張子(".txt")を返します。
    /// </summary>
    std::vector<std::string> SupportedExtensions() override;
    /// <summary>
    /// ObjectがTextAssetであるかどうかを判定します。
    /// </summary>
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    /// <summary>
    /// ファイルの内容をTextAssetとして読み込み、".meta"のユーザーデータをkey_value_pairsに読み込みます。
    /// </summary>
    void OnImport(AssetDescriptor *ctx) override;
    /// <summary>
    /// TextAssetのcontentをファイルに、key_value_pairsを".meta"のユーザーデータに書き出します。
    /// </summary>
    void OnExport(AssetDescriptor *ctx) override;
};
}