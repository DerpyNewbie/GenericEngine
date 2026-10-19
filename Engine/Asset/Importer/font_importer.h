#pragma once
#include "asset_importer.h"

namespace engine
{
/// <summary>
/// フォントファイルをFontDataとしてImportするImporterです。
/// </summary>
class FontImporter : public AssetImporter
{
public:
    /// <summary>
    /// 対応する拡張子(".spritefont")を返します。
    /// </summary>
    std::vector<std::string> SupportedExtensions() override;
    /// <summary>
    /// ObjectがFontDataであるかどうかを判定します。
    /// </summary>
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    /// <summary>
    /// フォントファイルを読み込み、FontDataをMainObjectとして設定します。
    /// </summary>
    void OnImport(AssetDescriptor *ctx) override;
};
}