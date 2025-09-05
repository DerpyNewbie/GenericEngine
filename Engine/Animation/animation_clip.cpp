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

Matrix TransformAnimationCurve::Evaluate(const float time) const
{
    const auto position = CalcLinerInterpolated(time, position_key);
    const auto scale = CalcLinerInterpolated(time, scale_key);
    const auto rotation = CalcLinerInterpolated(time, rotation_key);

    const auto result_matrix = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) *
                               Matrix::CreateTranslation(position);
    return result_matrix;
}

void AnimationClip::OnInspectorGui()
{
    Gui::PropertyField("Length", m_length_);
    Gui::PropertyField("FrameRate", m_frame_rate_);
}
std::unordered_map<std::string, TransformAnimationCurve> AnimationClip::GetCurves()
{
    return m_curves_;
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