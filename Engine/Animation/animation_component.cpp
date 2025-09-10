#include "pch.h"
#include "animation_component.h"

#include "engine_time.h"
#include "game_object.h"
#include "gui.h"
#include "Components/transform.h"

namespace engine
{

namespace
{
Quaternion Slerp(const Quaternion &a, const Quaternion &b, const float t)
{
    float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

    Quaternion end = b;
    if (dot < 0.0f)
    {
        dot = -dot;
        end = Quaternion(-b.x, -b.y, -b.z, -b.w);
    }

    if (Mathf::Approximately(dot, 1.0F))
    {
        auto result = Quaternion(
            a.x + t * (end.x - a.x),
            a.y + t * (end.y - a.y),
            a.z + t * (end.z - a.z),
            a.w + t * (end.w - a.w)
            );
        result.Normalize();
        return result;
    }

    const float theta_0 = acosf(dot);
    const float theta = theta_0 * t;

    const float sin_theta = sinf(theta);
    const float sin_theta_0 = sinf(theta_0);

    const float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;
    const float s1 = sin_theta / sin_theta_0;

    const auto result = Quaternion(
        (a.x * s0) + (end.x * s1),
        (a.y * s0) + (end.y * s1),
        (a.z * s0) + (end.z * s1),
        (a.w * s0) + (end.w * s1)
        );
    return result;
}

template <typename T>
T Lerp(float time, const std::vector<std::pair<float, T>> &keys, size_t &last_index)
{
    size_t i = last_index;

    while (i + 1 < keys.size() && keys[i + 1].first < time)
        ++i;

    last_index = i;

    if (i + 1 >= keys.size())
        return keys.back().second;

    const auto &[t1, v1] = keys[i];
    const auto &[t2, v2] = keys[i + 1];

    float factor = (time - t1) / (t2 - t1);
    return v1 + factor * (v2 - v1);
}
}

void AnimationComponent::AddTransform(const std::shared_ptr<Transform> &node)
{
    auto path = node->GameObject()->Name();
    m_transforms_.emplace(path, node);

    TRS trs;
    node->LocalMatrix().Decompose(trs.scale, trs.rotation, trs.translate);
    m_default_poses_.emplace(path, trs);
    for (UINT i = 0; i < node->ChildCount(); ++i)
    {
        auto child = node->GetChild(i);
        AddTransform(child);
    }
}

void AnimationComponent::OnInspectorGui()
{
    Gui::PropertyField("Clip", m_clip_);

    if (ImGui::TreeNode("States"))
    {
        for (auto [name, state] : m_states_)
        {
            if (ImGui::TreeNode(name.c_str()))
            {
                state->OnInspectorGui();
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    if (ImGui::Button("Play"))
    {
        Play();
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop"))
    {
        Stop();
    }
}
void AnimationComponent::OnStart()
{
    AddTransform(GameObject()->Transform());
}

void AnimationComponent::OnUpdate()
{
    Sample();
}

bool AnimationComponent::Play()
{
    const auto clip = m_clip_.CastedLock();
    if (clip == nullptr)
        return false;

    m_is_playing_ = true;
    for (const auto &state : m_states_ | std::ranges::views::values)
    {
        if (state->clip.CastedLock() == clip)
        {
            state->enabled = true;
            return true;
        }
    }

    AddClip(clip, clip->Name());
    return true;
}

bool AnimationComponent::Play(const std::string &name)
{
    const auto it = m_states_.find(name);
    if (it == m_states_.end() || it->second == nullptr)
    {
        Logger::Warn<AnimationComponent>("Animation state '%s' not found", name.c_str());
        return false;
    }

    it->second->enabled = true;
    m_is_playing_ = true;
    return true;
}

void AnimationComponent::Stop()
{
    bool all_disabled = true;
    for (const auto &state : m_states_ | std::views::values)
    {
        state->Stop();
        if (state->enabled == true)
        {
            all_disabled = false;
        }
    }

    if (all_disabled)
    {
        m_is_playing_ = false;
        for (auto &[path, transform] : m_transforms_)
            transform->SetLocalMatrix(m_default_poses_[path].GetMatrix());
    }
}

std::shared_ptr<AnimationState> AnimationComponent::AddClip(const std::shared_ptr<AnimationClip> &clip,
                                                            const std::string &name)
{
    const auto state = std::make_shared<AnimationState>();
    state->SetClip(clip);

    m_states_.insert_or_assign(name, state);
    return state;
}

std::shared_ptr<AnimationState> AnimationComponent::FindClip(const std::string &name)
{
    const auto it = m_states_.find(name);
    if (it == m_states_.end())
        return nullptr;
    return it->second;
}

void AnimationComponent::RemoveClip(const std::string &name)
{
    m_states_.erase(name);
}

size_t AnimationComponent::ClipCount() const
{
    return m_states_.size();
}

void AnimationComponent::Sample()
{
    if (m_states_.empty())
        return;

    float base_weight = 0.0f;
    bool enabled = false;
    for (const auto &state : m_states_ | std::views::values)
    {
        if (state->enabled)
        {
            state->UpdateTime();
            base_weight += state->weight;
            enabled = true;
        }
    }
    if (!enabled)
        return;

    base_weight = 1.0f - base_weight;

    for (auto &[path, transform] : m_transforms_)
    {
        auto &default_matrix = m_default_poses_[path];
        TRS final_trs;
        final_trs.translate = default_matrix.translate * base_weight;
        final_trs.scale = default_matrix.scale * base_weight;

        final_trs.rotation = default_matrix.rotation;
        float total_rot_weight = base_weight;

        for (const auto &state : m_states_ | std::views::values)
        {
            if (!state->enabled)
                continue;

            const auto clip = state->clip.CastedLock();
            const auto curve = clip->FindCurve(path);

            if (curve == nullptr)
            {
                final_trs.translate += default_matrix.translate * state->weight;
                final_trs.scale += default_matrix.scale * state->weight;

                const float t = state->weight / (total_rot_weight + state->weight);
                final_trs.rotation = Slerp(final_trs.rotation, default_matrix.rotation, t);
                total_rot_weight += state->weight;
                continue;
            }

            auto time = state->GetTime();
            final_trs.translate += Lerp(time, curve->position_key, curve->position_index) * state->weight;
            final_trs.scale += Lerp(time, curve->scale_key, curve->scale_index) * state->weight;

            Quaternion rot = Lerp(time, curve->rotation_key, curve->rotation_index);
            const float t = state->weight / (total_rot_weight + state->weight);
            final_trs.rotation = Slerp(final_trs.rotation, rot, t);
            total_rot_weight += state->weight;
        }

        transform->SetLocalMatrix(final_trs.GetMatrix());
    }
}

bool AnimationComponent::IsPlaying() const
{
    return m_is_playing_;
}
}