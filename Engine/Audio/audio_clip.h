#pragma once
#include "Asset/inspectable_asset.h"
#include <directxtk12/Audio.h>

namespace engine
{
class AudioClip : public InspectableAsset
{
    friend class Audio;

    std::unique_ptr<DirectX::SoundEffect> m_sound_effect_;

public:
    void OnInspectorGui() override;

    std::chrono::milliseconds Duration() const;
    unsigned short Channels() const;
    unsigned long SampleRate() const;
};
}