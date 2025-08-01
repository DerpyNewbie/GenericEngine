#include "pch.h"
#include "audio.h"

using namespace DirectX;

namespace engine
{
std::shared_ptr<Audio> Audio::Instance()
{
    if (!m_instance_)
    {
        m_instance_ = std::make_shared<Audio>();
    }
    return m_instance_;
}

void Audio::Init()
{
    auto instance = Instance();
    if (instance->m_audio_engine_)
        return;
    AUDIO_ENGINE_FLAGS flags = AudioEngine_Default;
#ifdef _DEBUG
    flags |= AudioEngine_Debug;
#endif

    instance->m_audio_engine_ = std::make_unique<AudioEngine>(flags);
}

void Audio::Subscribe(std::shared_ptr<AudioSource> audio_source)
{
    Instance()->m_audio_sources_.emplace_back(audio_source);
}

void Audio::UnSubscribe(const std::shared_ptr<AudioSource> &audio_source)
{
    auto instance = Instance();
    auto it = std::find(instance->m_audio_sources_.begin(), instance->m_audio_sources_.end(), audio_source);
    if (it != instance->m_audio_sources_.end())
    {
        instance->m_audio_sources_.erase(it);
    }
}
}