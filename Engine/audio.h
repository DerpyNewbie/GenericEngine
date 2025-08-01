#pragma once
#include "Components/audio_source.h"
#include <directxtk12/Audio.h>

namespace engine
{
class Audio
{
    std::shared_ptr<DirectX::AudioEngine> m_audio_engine_;
    std::list<std::shared_ptr<DirectX::SoundEffectInstance>> m_audio_sources_;

    static std::shared_ptr<Audio> m_instance_;
    static std::shared_ptr<Audio> Instance();

public:
    static void Init();
    static void Subscribe(std::shared_ptr<AudioSource> audio_source);
    static void UnSubscribe(const std::shared_ptr<AudioSource> &audio_source);

    static void SetListener(const Matrix &);

    static std::shared_ptr<DirectX::AudioEngine> GetAudioEngine()
    {
        return Instance()->m_audio_engine_;
    }

};
}