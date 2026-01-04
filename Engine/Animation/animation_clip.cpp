#include "pch.h"
#include "animation_clip.h"
#include "gui.h"

namespace engine
{

void AnimationClip::OnInspectorGui()
{
    Gui::PropertyField("Length", m_length_);
    Gui::PropertyField("FrameRate", m_frame_rate_);
}

TransformAnimationCurve *AnimationClip::FindCurve(const std::string &path)
{
    const auto it = m_curves_.find(path);
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

CEREAL_REGISTER_TYPE(engine::AnimationClip)