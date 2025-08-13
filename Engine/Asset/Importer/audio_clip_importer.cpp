#include "pch.h"

#include "audio_clip_importer.h"

#include "Audio/audio.h"
#include "Audio/audio_clip.h"

namespace engine
{
std::vector<std::string> AudioClipImporter::SupportedExtensions()
{
    return {".wav"};
}

std::shared_ptr<Object> AudioClipImporter::Import(std::istream &input_stream, AssetDescriptor *asset)
{
    return Audio::Instance()->LoadAudioClip(asset->guid, asset->path_hint);
}
}