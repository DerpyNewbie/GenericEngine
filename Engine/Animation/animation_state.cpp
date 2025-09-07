#include "pch.h"
#include "animation_state.h"

#include "engine_time.h"
#include "gui.h"

namespace engine
{
void AnimationState::OnInspectorGui()
{
    Gui::PropertyField("Enabled", enabled);
    Gui::PropertyField("Clip", clip);
    Gui::PropertyField("Name", name);
    Gui::PropertyField("Speed", speed);
    Gui::PropertyField("Time", time);
    Gui::PropertyField("Weight", weight);
    Gui::PropertyField("Length", length);

    int wrap_mode_int = static_cast<int>(wrap_mode);
    if (ImGui::Combo("Wrap Mode", &wrap_mode_int, "Once\0Loop\0PingPong\0\0"))
        wrap_mode = static_cast<kWrapMode>(wrap_mode_int);
}
void AnimationState::SetClip(std::shared_ptr<AnimationClip> clip)
{
    length = clip->Length();
    this->clip = AssetPtr<AnimationClip>::FromManaged(clip);
}

void AnimationState::UpdateTime()
{
    time += Time::GetDeltaTime() * speed;
    if (time > length && wrap_mode == kWrapMode::kLoop)
    {
        time = 0;
        clip.CastedLock()->Initialize();
    }
}

float AnimationState::NormalizedTime() const
{
    return Mathf::Approximately(length, 0) ? 0 : time / length;
}

float AnimationState::NormalizedSpeed() const
{
    return Mathf::Approximately(length, 0) ? 0 : speed / length;
}

void AnimationState::SetNormalizedTime(const float normalized_time)
{
    time = length * normalized_time;
}

void AnimationState::SetNormalizedSpeed(const float normalized_speed)
{
    speed = length * normalized_speed;
}

void AnimationState::Stop()
{
    enabled = false;
    time = 0;
    clip.CastedLock()->Initialize();
}
}