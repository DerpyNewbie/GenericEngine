#include "audio_emitter.h"

void engine::audio_emitter::Play(bool is_loop)
{
    audio_asset.CastedLock()->audio.Play(is_loop);
}

void engine::audio_emitter::SetVolume(float volume)
{
    audio_asset.CastedLock()->audio.SetVolume(volume);
}

DirectX::SoundState engine::audio_emitter::SoundState()
{
    return audio_asset.CastedLock()->audio.SoundState();
}