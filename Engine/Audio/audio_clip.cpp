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