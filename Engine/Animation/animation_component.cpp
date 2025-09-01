#include "pch.h"
#include "animation_component.h"
#include "game_object.h"
#include "gui.h"
#include "Components/transform.h"

namespace engine
{
namespace
{
std::shared_ptr<Transform> FindNodeRecursive(const std::shared_ptr<Transform> node, const std::string &name)
{
    if (node->GameObject()->Name() == name)
        return node;
    for (unsigned int i = 0; i < node->ChildCount(); ++i)
    {
        if (auto found = FindNodeRecursive(node->GetChild(i), name))
        {
            return found;
        }
    }
    return nullptr;
}
}

void AnimationComponent::OnInspectorGui()
{
    Gui::PropertyField("Clip", m_clip_);
    if (ImGui::Button("Play"))
    {
        Play();
    }
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

bool AnimationComponent::Play(const std::string &name)
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

void AnimationComponent::AddClip(std::shared_ptr<AnimationClip> clip, const std::string &name)
{
    const auto state = std::make_shared<AnimationState>();
    state->clip = AssetPtr<AnimationClip>::FromManaged(clip);

    const auto node_names = clip->GetNodeNames();
    for (auto node_name : node_names)
    {
        auto transform = FindNodeRecursive(GameObject()->Transform(), node_name);
        m_transforms_.emplace(node_name, transform);
    }
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
    for (auto [path, state] : m_states_)
    {
        state->clip.CastedLock()->GetSampledMatrix();
    }
}

bool AnimationComponent::IsPlaying() const
{
    return m_is_playing_;
}
}