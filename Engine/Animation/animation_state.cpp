#include "pch.h"
#include "animation_state.h"

#include "engine_time.h"
#include "gui.h"

namespace engine
{
void AnimationState::CreateCurvesCache(std::unordered_map<std::string, TransformAnimationCurve> &curves)
{
    for (auto &curve : curves | std::ranges::views::values)
    {
        curves_cache.emplace(&curve, AnimationCurveCache{});
    }
}

void AnimationState::ReleaseCurvesCache()
{
    for (auto &cache : curves_cache | std::ranges::views::values)
    {
        cache.position_index = 0;
        cache.rotation_index = 0;
        cache.scale_index = 0;
    }
}

void AnimationState::OnInspectorGui()
{
    Gui::PropertyField("Enabled", enabled);
    Gui::PropertyField("Name", name);
    Gui::PropertyField("Speed", speed);
    Gui::PropertyField("Time", time);
    Gui::PropertyField("Weight", weight);

    int wrap_mode_int = static_cast<int>(wrap_mode);
    if (ImGui::Combo("Wrap Mode", &wrap_mode_int, "Once\0Loop\0PingPong\0\0"))
        wrap_mode = static_cast<kWrapMode>(wrap_mode_int);
}

void AnimationState::SetClip(std::shared_ptr<AnimationClip> clip)
{
    length = clip->Length();
    this->clip = AssetPtr<AnimationClip>::FromManaged(clip);
    CreateCurvesCache(clip->m_curves_);
}

void AnimationState::UpdateTime()
{
    if (wrap_mode == kWrapMode::kOnce || wrap_mode == kWrapMode::kLoop)
    {
        time += Time::GetDeltaTime() * speed;
    }

    if (time > length && wrap_mode == kWrapMode::kLoop)
    {
        time = 0;
        just_looped = true;
        ReleaseCurvesCache();
        return;
    }
    just_looped = false;
    
    if (wrap_mode == kWrapMode::kPingPong)
    {
        if (time > length)
            time += Time::GetDeltaTime();
        ReleaseCurvesCache();
    }
}

float AnimationState::GetTime() const
{
    if (wrap_mode == kWrapMode::kOnce || wrap_mode == kWrapMode::kLoop)
        return time;
    float cycle = fmod(time, length * 2.0f);
    if (cycle > length)
        cycle = 2.0f * length - cycle;
    return cycle;
}

float AnimationState::NormalizedTime() const
{
    return Mathf::Approximately(length, 0) ? 0 : time / length;
}
bool AnimationState::HasEnded() const
{
    return wrap_mode == kWrapMode::kOnce && time >= length;
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
    ReleaseCurvesCache();
}
}