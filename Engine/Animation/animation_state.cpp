#include "pch.h"
#include "animation_state.h"
#include "gui.h"

namespace engine
{

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
}
}