#pragma once
#include "audio_asset.h"
#include "component.h"
#include "Asset/asset_ptr.h"

namespace engine
{
class AudioEmitterComponent : public Component
{
public:
    AssetPtr<AudioAsset> audio_asset;
    void Play(bool is_loop);
    void SetVolume(float volume);
    DirectX::SoundState SoundState();
};
}