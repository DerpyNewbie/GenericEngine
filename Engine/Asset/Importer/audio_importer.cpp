#include "pch.h"
#include "audio_importer.h"
#include "audio_asset.h"

std::vector<std::string> engine::AudioImporter::SupportedExtensions()
{
    return {".wav"};
}

std::shared_ptr<engine::Object> engine::AudioImporter::Import(std::istream &input_stream, AssetDescriptor *asset)
{
    std::shared_ptr<AudioAsset> audio_asset = Object::Instantiate<AudioAsset>(asset->guid);
    audio_asset->audio.LoadWav(asset->path_hint);

    return audio_asset;
}