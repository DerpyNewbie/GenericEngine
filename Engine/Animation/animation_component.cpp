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

void AnimationComponent::AddTransform(const std::shared_ptr<Transform> &node)
{
    m_transforms_.emplace(node->GameObject()->Name(), node);
    for (UINT i = 0; i < node->ChildCount(); ++i)
    {
        auto child = node->GetChild(i);
        AddTransform(child);
    }
}

void AnimationComponent::OnInspectorGui()
{
    Gui::PropertyField("Clip", m_clip_);
    if (ImGui::Button("Play"))
    {
        Play();
    }
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

    AddClip(clip, clip->Name());
    return true;
}

bool AnimationComponent::Play(const std::string &name) const
{
    const auto state = m_states_.at(name);
    if (state == nullptr)
        return false;

    state->enabled = true;
    return true;
}

void AnimationComponent::Stop()
{
    for (const auto &state : m_states_ | std::views::values)
        state->Stop();
}

void AnimationComponent::AddClip(const std::shared_ptr<AnimationClip> &clip, const std::string &name)
{
    const auto state = std::make_shared<AnimationState>();
    state->clip = AssetPtr<AnimationClip>::FromManaged(clip);

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

    std::vector<std::map<std::string, Matrix>> animations_matrices;
    for (const auto state : m_states_ | std::views::values)
    {
        if (!state->enabled)
        {
            continue;
        }
        auto clip = state->clip.CastedLock();
        auto curves = clip->GetCurves();

        state->time += Time::GetDeltaTime() * state->speed;

        std::map<std::string, Matrix> animation_matrices;
        for (auto [name, curve] : curves)
        {
            auto position = CalcLinerInterpolated(state->time, curve.position_key);
            auto scale = CalcLinerInterpolated(state->time, curve.scale_key);
            auto rotation = CalcLinerInterpolated(state->time, curve.rotation_key);

            auto matrix = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) *
                          Matrix::CreateTranslation(position);
            animation_matrices.emplace(name, matrix);
        }
        animations_matrices.emplace_back(animation_matrices);
    }

    for (auto [path, transform] : m_transforms_)
    {
        Matrix final_matrix;
        bool contains = false;
        for (auto animation_matrices : animations_matrices)
        {
            if (animation_matrices.contains(path))
            {
                final_matrix *= animation_matrices[path];
                contains = true;
            }

        }
        if (contains)
        {
            transform->SetLocalMatrix(final_matrix);
        }
    }
}

bool AnimationComponent::IsPlaying() const
{
    return m_is_playing_;
}
}