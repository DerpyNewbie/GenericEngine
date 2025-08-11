#include "pch.h"

#include "audio_source_component.h"
#include "audio.h"
#include "gui.h"

namespace engine
{
void AudioSourceComponent::OnStart()
{
    if (m_play_on_start_)
    {
        Play();
    }
}

void AudioSourceComponent::OnEnabled()
{
    if (m_sound_effect_instance_ == nullptr)
    {
        return;
    }

    m_sound_effect_instance_->Resume();
}
void AudioSourceComponent::OnUpdate()
{
    if (m_sound_effect_instance_ == nullptr)
    {
        return;
    }

    if (m_loop_ && m_sound_effect_instance_->GetState() == DirectX::STOPPED)
    {
        Play();
    }

    if (m_use_3d_ && m_sound_effect_instance_->GetState() == DirectX::PLAYING)
    {
        auto listener = Audio::Instance()->AudioListener();
        listener.SetVelocity(listener.Velocity * m_doppler_factor_);
        m_sound_effect_instance_->Apply3D(listener, m_emitter_);
    }
}

void AudioSourceComponent::OnDisabled()
{
    if (m_sound_effect_instance_ == nullptr)
    {
        return;
    }

    m_sound_effect_instance_->Pause();
}

void AudioSourceComponent::OnInspectorGui()
{
    Gui::PropertyField("Clip", m_clip_);

    Gui::PropertyField("Play On Start", m_play_on_start_);
    if (Gui::PropertyField("Volume", m_volume_))
    {
        SetVolume(m_volume_);
    }

    if (Gui::PropertyField("Pitch", m_pitch_))
    {
        SetPitch(m_pitch_);
    }

    Gui::PropertyField("Doppler Factor", m_doppler_factor_);
    Gui::PropertyField("Loop", m_loop_);

    if (Gui::PropertyField("Use 3D", m_use_3d_))
    {
        SetUse3D(m_use_3d_);
    }

    if (ImGui::Button("Play"))
    {
        Play();
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause"))
    {
        Pause();
    }
    ImGui::SameLine();
    if (ImGui::Button("Resume"))
    {
        Resume();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
    {
        Stop();
    }

    if (ImGui::CollapsingHeader("Audio Source State", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Is Playing: %s", IsPlaying() ? "Yes" : "No");
        ImGui::Text("Is Paused: %s", IsPaused() ? "Yes" : "No");
        ImGui::Text("Is Stopped: %s", IsStopped() ? "Yes" : "No");
        ImGui::Text("Is Looping: %s", IsLooping() ? "Yes" : "No");
    }
}

void AudioSourceComponent::SetClip(const AssetPtr<AudioClip> &clip)
{
    m_clip_ = clip;
    if (m_sound_effect_instance_ != nullptr)
    {
        m_sound_effect_instance_->Stop();
        m_sound_effect_instance_.reset();
    }
}

void AudioSourceComponent::SetVolume(const float volume)
{
    m_volume_ = std::abs(volume);
    if (m_sound_effect_instance_ != nullptr)
    {
        m_sound_effect_instance_->SetVolume(m_volume_);
    }
}

void AudioSourceComponent::SetPitch(const float pitch)
{
    m_pitch_ = Mathf::Clamp(pitch, -1.0f, 1.0f);
    if (m_sound_effect_instance_ != nullptr)
    {
        m_sound_effect_instance_->SetPitch(m_pitch_);
    }
}

void AudioSourceComponent::SetDopplerFactor(const float factor)
{
    m_doppler_factor_ = factor;
}

void AudioSourceComponent::SetLoop(const bool loop)
{
    m_loop_ = loop;
}

void AudioSourceComponent::SetUse3D(const bool use_3d)
{
    m_use_3d_ = use_3d;
    if (m_sound_effect_instance_ != nullptr)
    {
        Logger::Warn<AudioSourceComponent>(
            "Cannot dynamically change usage of 3D. Please stop and re-play its AudioSource to apply.");
    }
}

void AudioSourceComponent::SetPlayOnStart(const bool play_on_start)
{
    m_play_on_start_ = play_on_start;
}

AssetPtr<AudioClip> AudioSourceComponent::Clip() const
{
    return m_clip_;
}

float AudioSourceComponent::Volume() const
{
    return m_volume_;
}

float AudioSourceComponent::Pitch() const
{
    return m_pitch_;
}

bool AudioSourceComponent::IsLooping() const
{
    return m_loop_;
}

bool AudioSourceComponent::Use3D() const
{
    return m_use_3d_;
}

bool AudioSourceComponent::DoPlayOnStart() const
{
    return m_play_on_start_;
}

bool AudioSourceComponent::IsPlaying() const
{
    return m_sound_effect_instance_ != nullptr && m_sound_effect_instance_->GetState() == DirectX::PLAYING;
}

bool AudioSourceComponent::IsPaused() const
{
    return m_sound_effect_instance_ != nullptr && m_sound_effect_instance_->GetState() == DirectX::PAUSED;
}

bool AudioSourceComponent::IsStopped() const
{
    return m_sound_effect_instance_ == nullptr || m_sound_effect_instance_->GetState() == DirectX::STOPPED;
}

void AudioSourceComponent::Play()
{
    Audio::Instance()->Play(shared_from_base<AudioSourceComponent>());
}

void AudioSourceComponent::Pause() const
{
    if (m_sound_effect_instance_ == nullptr)
    {
        return;
    }

    m_sound_effect_instance_->Pause();
}

void AudioSourceComponent::Resume() const
{
    if (m_sound_effect_instance_ == nullptr)
    {
        return;
    }

    m_sound_effect_instance_->Resume();
}

void AudioSourceComponent::Stop()
{
    if (m_sound_effect_instance_ == nullptr)
    {
        return;
    }

    m_sound_effect_instance_->Stop();
    m_sound_effect_instance_ = nullptr;
}
}