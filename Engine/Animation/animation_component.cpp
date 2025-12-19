#include "pch.h"
#include "animation_component.h"

#include <assimp/MathFunctions.h>

#include "game_object.h"
#include "gui.h"
#include "Components/transform.h"

namespace engine
{

namespace
{
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
    node->LocalMatrix().Decompose(trs.scale, trs.rotation, trs.translation);
    m_default_poses_.emplace(path, trs);
    for (UINT i = 0; i < node->ChildCount(); ++i)
    {
        auto child = node->GetChild(i);
        AddTransform(child);
    }
}
void AnimationComponent::OnAwake()
{
    if (m_apply_root_motion_ && m_root_bone_.Lock() == nullptr)
    {
        Logger::Warn<AnimationComponent>("Root motion is enabled but no RootBone is assigned. Root motion will not function.");
    }
}

void AnimationComponent::OnInspectorGui()
{
    Gui::PropertyField("Root Bone", m_root_bone_);
    Gui::PropertyField("Clip", m_clip_);
    ImGui::Checkbox("Root Motion", &m_apply_root_motion_);

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

Vector3 AnimationComponent::GetDeltaPosition() const
{
    return m_delta_position_;
}

Quaternion AnimationComponent::GetDeltaRotation() const
{
    return m_delta_rotation_;
}

std::pair<AnimationComponent::StateIterator, bool> AnimationComponent::AddClip(
    const std::shared_ptr<AnimationClip> &clip,
    const std::string &name
)
{
    const auto state = std::make_shared<AnimationState>();
    state->SetClip(clip);

    return AddState(state, name);
}

std::pair<AnimationComponent::StateIterator, bool> AnimationComponent::AddState(std::shared_ptr<AnimationState> state, const std::string &name)
{
    return m_states_.insert_or_assign(name, state);
}

std::shared_ptr<AnimationState> AnimationComponent::FindClip(const std::string &name) const
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
        m_is_first_frames_[state] = false;
        m_is_prev_frame_enabled_[state] = state->enabled;
        if (state->enabled)
        {
            if (!m_is_prev_frame_enabled_[state])
                m_is_first_frames_[state] = true;
                
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
        if (transform == GameObject()->Transform())
            continue;
        
        auto &default_matrix = m_default_poses_[path];
        TRS final_trs;
        final_trs.translation = default_matrix.translation * base_weight;
        final_trs.scale = default_matrix.scale * base_weight;

        final_trs.rotation = Mathf::Slerp(Quaternion::Identity, default_matrix.rotation, base_weight);
        float total_rot_weight = base_weight;

        for (const auto &state : m_states_ | std::views::values)
        {
            if (!state->enabled)
                continue;

            const auto clip = state->clip.CastedLock();
            const auto curve = clip->FindCurve(path);

            total_rot_weight += state->weight;
            const float t = Mathf::Approximately(total_rot_weight, 0)
                ? state->weight
                : state->weight / total_rot_weight;
            if (curve == nullptr)
            {
                final_trs.translation += default_matrix.translation * state->weight;
                final_trs.scale += default_matrix.scale * state->weight;

                final_trs.rotation = Mathf::Lerp(final_trs.rotation, default_matrix.rotation, t);
                continue;
            }

            const auto time = state->GetTime();
            auto pos = Lerp(time, curve->position_key, curve->position_index);
            auto rot = Lerp(time, curve->rotation_key, curve->rotation_index);
            auto scale = Lerp(time, curve->scale_key, curve->scale_index);

            if (transform == m_root_bone_.CastedLock())
            {
                if (state->just_looped || m_is_first_frames_[state])
                {
                    m_previous_positions_[state] = pos;
                    m_previous_rotations_[state] = rot;
                    continue;
                }
                
                auto delta_pos = pos - m_previous_positions_[state];
                delta_pos.y = 0;
                
                Quaternion prev_inv;
                m_previous_rotations_[state].Inverse(prev_inv);

                auto delta_rot = prev_inv * rot;
                auto euler = delta_rot.ToEuler();
                Quaternion yaw_only = Quaternion::CreateFromYawPitchRoll(euler.y, 0.0f, 0.0f);
                
                m_previous_positions_[state] = pos;
                m_previous_rotations_[state] = rot;
                
                m_delta_position_ += delta_pos * t;
                m_delta_rotation_ = Mathf::Slerp(Quaternion::Identity, yaw_only, t) * m_delta_rotation_;
                m_delta_rotation_.Normalize();

                if (m_apply_root_motion_)
                {
                    pos.x = 0;
                    pos.z = 0;

                    float yaw = rot.ToEuler().y;

                    Quaternion yaw_q = Quaternion::CreateFromYawPitchRoll(yaw, 0.0f, 0.0f);
                
                    Quaternion yaw_inv;
                    yaw_q.Inverse(yaw_inv);

                    rot = rot * yaw_inv;
                }
            }

            final_trs.translation += pos * t;
            final_trs.rotation = Mathf::Slerp(final_trs.rotation, rot, t);
            final_trs.scale += scale * t;
        }

        transform->SetLocalPosition(final_trs.translation);
        transform->SetLocalRotation(final_trs.rotation);
        transform->SetLocalScale(final_trs.scale);
    }

    auto owner_transform = GameObject()->Transform();
    if (auto parent = owner_transform->Parent())
    {
        m_delta_position_ = Vector3::Transform(m_delta_position_, parent->Rotation());
    }

    //BUG: ローカル空間しか想定してないからよ
    if (m_apply_root_motion_)
    {
        owner_transform->SetLocalPosition(owner_transform->LocalPosition() + m_delta_position_ * owner_transform->Scale());
        owner_transform->SetLocalRotation(owner_transform->LocalRotation() * m_delta_rotation_);
    }
    m_delta_position_ = Vector3::Zero;
    m_delta_rotation_ = Quaternion::Identity;
}

bool AnimationComponent::IsPlaying() const
{
    return m_is_playing_;
}
}

CEREAL_REGISTER_TYPE(engine::AnimationComponent)