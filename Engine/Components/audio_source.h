#pragma once
#include "component.h"
#include "Asset/asset_ptr.h"
#include "Asset/audio_clip.h"

#include <directxtk12/Audio.h>

namespace engine
{
class AudioSource : Component
{
    bool use_3d = false;
    bool is_loop_ = false;
    float m_volume_ = 0;
    std::unique_ptr<DirectX::SoundEffectInstance> m_sound_instance_;

public:
    AssetPtr<AudioClip> audio_clip;

    void OnInspectorGui() override;
    void Play() const;

    void SetListenerMatrix(const Matrix &listener_matrix);
    bool IsPlaying() const;
};
}