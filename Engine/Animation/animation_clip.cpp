#include "pch.h"
#include "animation_clip.h"
#include "gui.h"

namespace engine
{
namespace
{
template <typename T>
T CalcLinerInterpolated(float time, const std::list<std::pair<float, T>> &vector_key)
{
    if (vector_key.size() == 1)
        return vector_key.begin()->second;

    auto next_it = vector_key.begin();
    for (auto it = vector_key.begin(); it != vector_key.end(); ++it)
    {
        if (it->first < time)
        {
            continue;
        }
        next_it = it;
        break;
    }

    if (next_it == vector_key.begin())
    {
        return next_it->second;
    }
    if (next_it == vector_key.end())
    {
        return std::prev(next_it)->second;
    }

    auto prevIt = std::prev(next_it);

    const float t1 = prevIt->first;
    const float t2 = next_it->first;
    const float delta_time = t2 - t1;

    if (delta_time <= 0.0f)
        return prevIt->second;

    float factor = (time - t1) / delta_time;

    const T &start = prevIt->second;
    const T &end = next_it->second;

    return start + factor * (end - start);
}
}

void AnimationClip::OnInspectorGui()
{
    Gui::PropertyField("Length", m_length_);
    Gui::PropertyField("FrameRate", m_frame_rate_);
}
void AnimationClip::Initialize()
{
    for (auto &curve : m_curves_ | std::ranges::views::values)
    {
        curve.position_index = 0;
        curve.rotation_index = 0;
        curve.scale_index = 0;
    }
}

TransformAnimationCurve *AnimationClip::FindCurve(const std::string &path)
{
    auto it = m_curves_.find(path);
    if (it == m_curves_.end())
        return nullptr;
    return &it->second;
}

float AnimationClip::Length() const
{
    return m_length_;
}

float AnimationClip::FrameRate() const
{
    return m_frame_rate_;
}
}