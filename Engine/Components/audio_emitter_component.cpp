#include "pch.h"

#include "audio_emitter_component.h"

void engine::AudioEmitterComponent::Play(bool is_loop)
{
    audio_asset.CastedLock()->audio.Play(is_loop);
}

void engine::AudioEmitterComponent::SetVolume(float volume)
{
    audio_asset.CastedLock()->audio.SetVolume(volume);
}

DirectX::SoundState engine::AudioEmitterComponent::SoundState()
{
    return audio_asset.CastedLock()->audio.SoundState();
}