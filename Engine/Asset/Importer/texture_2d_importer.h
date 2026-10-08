#pragma once
#include "asset_importer.h"
#include "Rendering/CabotEngine/Graphics/Texture2D.h"

namespace engine
{
/// <summary>
/// 画像ファイルをTexture2DとしてImportするImporterです。
/// </summary>
class Texture2DImporter : public AssetImporter
{
    /// <summary>
    /// 画像ファイルの形式です。
    /// </summary>
    enum class kImageFormat : uint8_t
    {
        kUnknown = 0,
        kWic,
        kTga
    };

    /// <summary>
    /// 拡張子から画像の読み込み方式(WICまたはTGA)を判定します。
    /// </summary>
    static kImageFormat GetImageFormat(const std::filesystem::path &file_path);

public:
    /// <summary>
    /// 指定された色で塗りつぶされた 4x4 のTexture2Dを生成します。
    /// </summary>
    /// <param name="color">塗りつぶす色</param>
    static IAssetPtr GetColorTexture(DirectX::PackedVector::XMCOLOR color);

    /// <summary>
    /// 対応する拡張子(".png"、".jpg"、".tga"など)を返します。
    /// </summary>
    std::vector<std::string> SupportedExtensions() override;
    /// <summary>
    /// 画像ファイルを読み込み、Texture2DをMainObjectとして設定します。
    /// </summary>
    void OnImport(AssetDescriptor *ctx) override;
};
}