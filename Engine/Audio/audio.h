#pragma once
#include "audio_clip.h"
#include "audio_source_component.h"
#include "event_receivers.h"

#include <directxtk12/Audio.h>

namespace engine
{
class Audio : public IUpdateReceiver
{
    friend class Engine;
    static std::shared_ptr<Audio> m_instance_;

    std::unique_ptr<DirectX::AudioEngine> m_audio_engine_;
    DirectX::AudioListener m_listener_;
    float m_doppler_factor_ = 1.0f;

    static void Init();

public:
    Audio();

    static std::shared_ptr<Audio> Instance();

    void OnUpdate() override;

    void Play(const std::shared_ptr<AudioSourceComponent> &audio_source);

    [[nodiscard]] float MasterVolume() const;
    [[nodiscard]] float DopplerFactor() const;
    [[nodiscard]] DirectX::AudioStatistics Statistics() const;
    [[nodiscard]] DirectX::AudioListener AudioListener() const;

    void SetMasterVolume(float volume) const;
    void SetDopplerFactor(float factor);

    [[nodiscard]] std::shared_ptr<AudioClip> LoadAudioClip(xg::Guid guid, const std::filesystem::path &path) const;
};
}