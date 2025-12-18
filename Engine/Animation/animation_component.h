#pragma once
#include "animation_clip.h"
#include "animation_state.h"
#include "Asset/asset_ptr.h"
#include "Asset/fbx_meta.h"
#include "Components/component.h"
#include "Components/transform.h"
#include "Math/trs.h"

namespace engine
{
class AnimationComponent : public Component
{
    /// <summary>
    /// Default animation clip
    /// </summary>
    AssetPtr<AnimationClip> m_clip_;
    AssetPtr<FbxMeta> m_fbx_meta_;
    std::string m_root_bone_name_;

    /// <summary>
    /// Should AnimationComponent play the default animation at startup?
    /// </summary>
    bool m_play_automatically_ = true;
    bool m_is_playing_ = false;
    bool m_apply_root_motion_ = false;

    std::unordered_map<std::shared_ptr<AnimationState>, Vector3> m_previous_positions_ = {};
    std::unordered_map<std::shared_ptr<AnimationState>, Quaternion> m_previous_rotations_ = {};

    Vector3 m_delta_position_;
    Quaternion m_delta_rotation_;

    using StateMap = std::unordered_map<std::string, std::shared_ptr<AnimationState>>;
    using StateIterator = StateMap::iterator;

    std::unordered_map<std::string, std::shared_ptr<Transform>> m_transforms_;
    std::unordered_map<std::string, TRS> m_default_poses_;
    StateMap m_states_;

    void AddTransform(const std::shared_ptr<Transform> &node);
    
public:
    void OnAwake() override;
    void OnInspectorGui() override;
    void OnStart() override;
    void OnUpdate() override;
    bool Play();
    bool Play(const std::string &name);
    void Stop();

    [[nodiscard]] Vector3 GetDeltaPosition() const;
    Quaternion GetDeltaRotation() const;
    
    std::pair<StateIterator, bool> AddClip(const std::shared_ptr<AnimationClip> &clip, const std::string &name);
    std::pair<StateIterator, bool> AddState(std::shared_ptr<AnimationState> state, const std::string &name);
    std::shared_ptr<AnimationState> FindClip(const std::string &name) const;
    void RemoveClip(const std::string &name);
    [[nodiscard]] size_t ClipCount() const;

    void Sample();

    [[nodiscard]] bool IsPlaying() const;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Component>(this),
            CEREAL_NVP(m_clip_),
            CEREAL_NVP(m_play_automatically_),
            CEREAL_NVP(m_is_playing_)
        );

        if (version >= 2)
        {
            ar(
                CEREAL_NVP(m_apply_root_motion_)
            );

        }
    }
};
}

CEREAL_CLASS_VERSION(engine::AnimationComponent, 2)