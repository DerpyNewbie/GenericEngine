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

bool AudioClipImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<AudioClip>(object) != nullptr;
}

void AudioClipImporter::OnImport(AssetDescriptor *ctx)
{
    const auto audio = Audio::Instance();
    if (audio == nullptr)
    {
        ctx->LogImportError("Audio not initialized");
        return;
    }

    ctx->SetMainObject(audio->LoadAudioClip(xg::newGuid(), ctx->AssetPath()));
}

}