#include "audio_asset.h"

CEREAL_REGISTER_TYPE(engine::AudioAsset)

void engine::AudioAsset::OnConstructed()
{
    audio.Initialize();
}