#pragma once
#include "animation_clip.h"
#include "animation_state.h"
#include "Asset/asset_ptr.h"
#include "Components/component.h"
#include "Components/transform.h"

namespace engine
{
class AnimationComponent : public Component
{
    /// <summary>
    /// Default animation clip
    /// </summary>
    AssetPtr<AnimationClip> m_clip_;

    /// <summary>
    /// Should AnimationComponent play the default animation at startup?
    /// </summary>
    bool m_play_automatically_ = true;
    bool m_is_playing_ = false;

    std::set<std::pair<std::string, std::shared_ptr<Transform>>> m_transforms_;
    std::unordered_map<std::string, std::shared_ptr<AnimationState>> m_states_;

public:
    void OnInspectorGui() override;
    void OnUpdate() override;
    bool Play();
    bool Play(const std::string &name);
    void Stop();

    void AddClip(std::shared_ptr<AnimationClip> clip, const std::string &name);
    void RemoveClip(const std::string &name);
    [[nodiscard]] size_t ClipCount() const;

    void Sample();

    [[nodiscard]] bool IsPlaying() const;
};
}