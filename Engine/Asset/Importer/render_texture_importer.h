#pragma once
#include "asset_importer.h"

namespace engine
{
/// <summary>
/// RenderTextureのImport / Exportを行うImporterです。
/// </summary>
class RenderTextureImporter : public AssetImporter
{
public:
    /// <summary>
    /// 対応する拡張子(".rendertexture")を返します。
    /// </summary>
    std::vector<std::string> SupportedExtensions() override;
    /// <summary>
    /// ObjectがRenderTextureであるかどうかを判定します。
    /// </summary>
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    /// <summary>
    /// 新しいRenderTextureを生成し、MainObjectとして設定します。
    /// </summary>
    void OnImport(AssetDescriptor *ctx) override;
    /// <summary>
    /// MainObjectのRenderTextureをSerializeしてファイルに書き出します。
    /// </summary>
    void OnExport(AssetDescriptor *ctx) override;
};
}