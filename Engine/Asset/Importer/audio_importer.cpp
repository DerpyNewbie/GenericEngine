#include "audio_importer.h"
#include "audio_asset.h"

std::vector<std::string> AudioImporter::SupportedExtensions()
{
    return {".wav"};
}

std::shared_ptr<engine::Object> AudioImporter::Import(std::istream &input_stream, engine::AssetDescriptor *asset)
{
    std::shared_ptr<engine::AudioAsset> audio_asset = engine::Object::Instantiate<engine::AudioAsset>(asset->guid);
    audio_asset->audio.LoadWav(asset->path_hint);

    return audio_asset;
}