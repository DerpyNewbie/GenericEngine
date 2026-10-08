#pragma once
#include "asset_importer.h"

namespace engine
{
/// <summary>
/// 音声ファイルをAudioClipとしてImportするImporterです。
/// </summary>
class AudioClipImporter : public AssetImporter
{
public:
    /// <summary>
    /// 対応する拡張子(".wav")を返します。
    /// </summary>
    std::vector<std::string> SupportedExtensions() override;
    /// <summary>
    /// ObjectがAudioClipであるかどうかを判定します。
    /// </summary>
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    /// <summary>
    /// 音声ファイルを読み込み、AudioClipをMainObjectとして設定します。
    /// </summary>
    void OnImport(AssetDescriptor *ctx) override;
};
}