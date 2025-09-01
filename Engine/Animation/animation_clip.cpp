#include "pch.h"
#include "animation_clip.h"
#include "gui.h"

namespace engine
{
Matrix TransformAnimationCurve::Evaluate(float time)
{
    return Matrix::Identity;
}

void AnimationClip::OnInspectorGui()
{
    Gui::PropertyField("Length", m_length_);
    Gui::PropertyField("FrameRate", m_frame_rate_);
}

std::vector<std::string> AnimationClip::GetNodeNames()
{
    std::vector<std::string> result;
    for (const auto key : m_curves_ | std::views::keys)
    {
        result.emplace_back(key);
    }

    return result;
}

Matrix AnimationClip::GetSampledMatrix(const float time, const std::string &path)
{
    return m_curves_[path].Evaluate(time);
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