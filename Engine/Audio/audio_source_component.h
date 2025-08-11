#pragma once
#include "audio_clip.h"
#include "Asset/asset_ptr.h"
#include "Components/component.h"

namespace engine
{
class AudioSourceComponent : public Component
{
    friend class Audio;

    std::shared_ptr<DirectX::SoundEffectInstance> m_sound_effect_instance_;
    DirectX::AudioEmitter m_emitter_;
    AssetPtr<AudioClip> m_clip_;
    float m_volume_ = 1.0f;
    float m_pitch_ = 0;
    float m_doppler_factor_ = 1.0f;
    bool m_loop_ = false;
    bool m_use_3d_ = true;
    bool m_play_on_start_ = false;

public:
    void OnStart() override;
    void OnEnabled() override;
    void OnUpdate() override;
    void OnDisabled() override;

    void OnInspectorGui() override;

    void SetClip(const AssetPtr<AudioClip> &clip);
    void SetVolume(float volume);
    void SetPitch(float pitch);
    void SetDopplerFactor(float factor);
    void SetLoop(bool loop);
    void SetUse3D(bool use_3d);
    void SetPlayOnStart(bool play_on_start);

    [[nodiscard]] AssetPtr<AudioClip> Clip() const;
    [[nodiscard]] float Volume() const;
    [[nodiscard]] float Pitch() const;
    [[nodiscard]] bool IsLooping() const;
    [[nodiscard]] bool Use3D() const;
    [[nodiscard]] bool DoPlayOnStart() const;

    [[nodiscard]] bool IsPlaying() const;
    [[nodiscard]] bool IsPaused() const;
    [[nodiscard]] bool IsStopped() const;

    void Play();
    void Pause() const;
    void Resume() const;
    void Stop();
};
}