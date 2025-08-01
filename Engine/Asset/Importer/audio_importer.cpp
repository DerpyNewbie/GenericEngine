#include "pch.h"
#include "audio_importer.h"

#include "audio.h"
#include "Asset/audio_clip.h"
#include "Rendering/CabotEngine/Engine/audio.h"

std::vector<std::string> engine::AudioImporter::SupportedExtensions()
{
    return {".wav"};
}

std::shared_ptr<engine::Object> engine::AudioImporter::Import(std::istream &input_stream, AssetDescriptor *asset)
{
    std::shared_ptr<AudioClip> audio_asset = Object::Instantiate<AudioClip>(asset->guid);
    audio_asset->m_sound_effect_ = std::make_unique<DirectX::SoundEffect>(
        Audio::GetAudioEngine().get(), asset->path_hint.c_str());

    return audio_asset;
}