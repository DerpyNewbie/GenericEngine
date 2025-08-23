#include "pch.h"

#include "audio_clip.h"

#include "str_util.h"

namespace engine
{
void AudioClip::OnInspectorGui()
{
    ImGui::Text("Duration: %s", StringUtil::DurationToString(Duration()).c_str());
    ImGui::Text("Channels: %d", Channels());
    ImGui::Text("Sample Rate: %d", SampleRate());

    static std::unique_ptr<DirectX::SoundEffectInstance> preview_instance;
    if (preview_instance == nullptr && ImGui::Button("Play"))
    {
        preview_instance = m_sound_effect_->CreateInstance();
        preview_instance->Play();
    }
    if (preview_instance != nullptr)
    {
        if (ImGui::Button("Stop"))
        {
            preview_instance->Stop();
            preview_instance.reset();
        }

        ImGui::SameLine();
        if (ImGui::Button("Pause"))
        {
            preview_instance->Pause();
        }

        ImGui::SameLine();
        if (ImGui::Button("Resume"))
        {
            preview_instance->Resume();
        }
    }
}

std::chrono::milliseconds AudioClip::Duration() const
{
    return std::chrono::milliseconds(m_sound_effect_->GetSampleDurationMS());
}
unsigned short AudioClip::Channels() const
{
    return m_sound_effect_->GetFormat()->nChannels;
}
unsigned long AudioClip::SampleRate() const
{
    return m_sound_effect_->GetFormat()->nSamplesPerSec;
}
}