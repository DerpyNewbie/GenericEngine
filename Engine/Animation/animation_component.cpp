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
Quaternion Slerp(const Quaternion &a, const Quaternion &b, float t)
{
    // 内積を計算
    float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

    // 最近接の経路を取るために符号を反転
    Quaternion end = b;
    if (dot < 0.0f)
    {
        dot = -dot;
        end = Quaternion(-b.x, -b.y, -b.z, -b.w);
    }

    // もしすごく近ければ Lerp で近似
    if (dot > 0.9995f)
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

    // 角度と補間比率を計算
    float theta_0 = acosf(dot); // a と b の角度
    float theta = theta_0 * t; // 補間後の角度

    float sin_theta = sinf(theta);
    float sin_theta_0 = sinf(theta_0);

    float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;

    auto result = Quaternion(
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

    // time が進んでいれば次に進める
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

Quaternion BlendRotations(const std::vector<std::pair<Quaternion, float>> &rotations)
{
    Quaternion result = Quaternion::Identity;
    float totalWeight = 0.0f;

    for (auto &[q, w] : rotations)
    {
        if (w <= 0)
            continue;
        if (totalWeight == 0.0f)
        {
            result = q;
            totalWeight = w;
        }
        else
        {
            float t = w / (totalWeight + w);
            result = Slerp(result, q, t);
            totalWeight += w;
        }
    }
    return result;
}
}

void AnimationComponent::AddTransform(const std::shared_ptr<Transform> &node)
{
    auto path = node->GameObject()->Name();
    m_transforms_.emplace(path, node);
    m_default_poses_.emplace(path, node->LocalMatrix());
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

    for (auto &state : m_states_ | std::ranges::views::values)
    {
        if (state->clip.CastedLock() == clip)
        {
            state->enabled = true;
            m_is_playing_ = true;
            return true;
        }
    }

    AddClip(clip, clip->Name());
    m_is_playing_ = true;
    return true;
}

bool AnimationComponent::Play(const std::string &name)
{
    const auto state = m_states_.at(name);
    if (state == nullptr)
        return false;

    state->enabled = true;
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
            transform->SetLocalMatrix(m_default_poses_[path]);
    }
}

void AnimationComponent::AddClip(const std::shared_ptr<AnimationClip> &clip, const std::string &name)
{
    const auto state = std::make_shared<AnimationState>();
    state->SetClip(clip);

    m_states_.insert_or_assign(name, state);
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
    for (auto state : m_states_ | std::views::values)
    {
        if (state->enabled)
        {
            state->UpdateTime();
            base_weight += state->weight;
        }
    }

    base_weight = 1.0f - base_weight;

    for (auto &[path, transform] : m_transforms_)
    {
        Vector3 default_pos, default_scale;
        Quaternion default_rot;
        m_default_poses_[path].Decompose(default_scale, default_rot, default_pos);

        Vector3 final_pos = default_pos * base_weight;
        Vector3 final_scale = default_scale * base_weight;

        Quaternion final_rot = default_rot;
        float total_rot_weight = base_weight;

        for (auto &state : m_states_ | std::views::values)
        {
            if (!state->enabled)
                continue;

            auto clip = state->clip.CastedLock();
            auto curve = clip->FindCurve(path);

            if (curve == nullptr)
            {
                final_pos += default_pos * state->weight;
                final_scale += default_scale * state->weight;

                float t = state->weight / (total_rot_weight + state->weight);
                final_rot = Slerp(final_rot, default_rot, t);
                total_rot_weight += state->weight;
                continue;
            }

            final_pos += Lerp(state->time, curve->position_key, curve->position_index) * state->weight;
            final_scale += Lerp(state->time, curve->scale_key, curve->scale_index) * state->weight;

            Quaternion rot = Lerp(state->time, curve->rotation_key, curve->rotation_index);
            float t = state->weight / (total_rot_weight + state->weight);
            final_rot = Slerp(final_rot, rot, t);
            total_rot_weight += state->weight;
        }

        auto final_matrix = Matrix::CreateScale(final_scale) *
                            Matrix::CreateFromQuaternion(final_rot) *
                            Matrix::CreateTranslation(final_pos);

        transform->SetLocalMatrix(final_matrix);
    }
}

bool AnimationComponent::IsPlaying() const
{
    return m_is_playing_;
}
}