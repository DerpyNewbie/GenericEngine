#include "pch.h"

#include "audio.h"

#include "audio_listener_component.h"
#include "engine_time.h"
#include "game_object.h"
#include "update_manager.h"

namespace engine
{
std::shared_ptr<Audio> Audio::m_instance_;

void Audio::Init()
{
    m_instance_ = std::make_shared<Audio>();
    UpdateManager::SubscribeUpdate(m_instance_);
}

Audio::Audio()
{
    m_audio_engine_ = std::make_unique<DirectX::AudioEngine>();
    m_listener_ = DirectX::AudioListener();
}

std::shared_ptr<Audio> Audio::Instance()
{
    return m_instance_;
}

void Audio::OnUpdate()
{
    const auto listener = AudioListenerComponent::ActiveListener();
    if (listener == nullptr)
    {
        m_listener_.SetPosition(Vector3::Zero);
        m_listener_.SetOrientationFromQuaternion(Quaternion::Identity);
        return;
    }

    const auto transform = listener->GameObject()->Transform();
    m_listener_.Update(transform->Position(), transform->Up(), Time::GetDeltaTime());
    Quaternion rot_inv;
    transform->Rotation().Inverse(rot_inv);
    m_listener_.SetOrientationFromQuaternion(rot_inv);
    m_listener_.SetVelocity(m_listener_.Velocity * m_doppler_factor_);

    m_audio_engine_->Update();
}


void Audio::Play(const std::shared_ptr<AudioSourceComponent> &audio_source)
{
    const auto clip = audio_source->m_clip_.CastedLock();
    if (clip == nullptr)
    {
        return;
    }

    auto instance_flags = DirectX::SoundEffectInstance_Default;
    if (audio_source->m_use_3d_)
    {
        instance_flags |= DirectX::SoundEffectInstance_ReverbUseFilters;
        instance_flags |= DirectX::SoundEffectInstance_Use3D;
    }

    const std::shared_ptr sound_effect_instance = clip->m_sound_effect_->CreateInstance(instance_flags);
    if (audio_source->m_use_3d_)
    {
        sound_effect_instance->Apply3D(m_listener_, audio_source->m_emitter_);
    }

    sound_effect_instance->SetPitch(audio_source->m_pitch_);
    sound_effect_instance->SetVolume(audio_source->m_volume_);
    sound_effect_instance->Play(audio_source->m_loop_);

    audio_source->m_sound_effect_instance_ = sound_effect_instance;
}

float Audio::MasterVolume() const
{
    return m_audio_engine_->GetMasterVolume();
}

float Audio::DopplerFactor() const
{
    return m_doppler_factor_;
}

DirectX::AudioStatistics Audio::Statistics() const
{
    return m_audio_engine_->GetStatistics();
}

DirectX::AudioListener Audio::AudioListener() const
{
    return m_listener_;
}

void Audio::SetMasterVolume(const float volume) const
{
    m_audio_engine_->SetMasterVolume(volume);
}

void Audio::SetDopplerFactor(const float factor)
{
    m_doppler_factor_ = factor;
}

std::shared_ptr<AudioClip> Audio::LoadAudioClip(const xg::Guid guid, const std::filesystem::path &path) const
{
    if (m_audio_engine_ == nullptr)
    {
        Logger::Error<Audio>("Audio engine is not initialized.");
        return nullptr;
    }

    const auto clip = Object::Instantiate<AudioClip>(guid);
    clip->m_sound_effect_ = std::make_unique<DirectX::SoundEffect>(m_audio_engine_.get(), path.c_str());
    return clip;
}
}