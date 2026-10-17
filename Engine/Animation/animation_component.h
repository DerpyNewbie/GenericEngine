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
/// <summary>
/// AnimationClipを再生し、GameObject以下のTransformを動かすComponentです。
/// </summary>
class AnimationComponent : public Component
{
    /// <summary>
    /// Default animation clip
    /// </summary>
    AssetPtr<AnimationClip> m_clip_;
    AssetPtr<Transform> m_root_bone_;

    /// <summary>
    /// Should AnimationComponent play the default animation at startup?
    /// </summary>
    bool m_play_automatically_ = true;
    bool m_is_playing_ = false;
    bool m_apply_root_motion_ = false;

    std::unordered_map<std::shared_ptr<AnimationState>, bool> m_is_prev_frame_enabled_;
    std::unordered_map<std::shared_ptr<AnimationState>, bool> m_is_first_frames_;
    std::unordered_map<std::shared_ptr<AnimationState>, Vector3> m_previous_positions_ = {};
    std::unordered_map<std::shared_ptr<AnimationState>, Quaternion> m_previous_rotations_ = {};

    Vector3 m_delta_position_;
    Quaternion m_delta_rotation_;

    using StateMap = std::unordered_map<std::string, std::shared_ptr<AnimationState>>;
    using StateIterator = StateMap::iterator;

    std::unordered_map<std::string, std::shared_ptr<Transform>> m_transforms_;
    std::unordered_map<std::string, TRS> m_default_poses_;
    StateMap m_states_;

    /// <summary>
    /// 指定されたTransformとそのすべての子を、GameObjectの名前をキーとして登録し、現在の姿勢を初期姿勢として記録します。
    /// </summary>
    /// <param name="node">登録するTransform</param>
    void AddTransform(const std::shared_ptr<Transform> &node);
    
public:
    /// <summary>
    /// RootMotionが有効なのにRootBoneが設定されていない場合に警告を出します。
    /// </summary>
    void OnAwake() override;
    void OnInspectorGui() override;
    /// <summary>
    /// 自身のGameObject以下のすべてのTransformをAnimationの対象として登録します。
    /// </summary>
    void OnStart() override;
    void OnUpdate() override;
    /// <summary>
    /// 既定のClipを再生します。Stateがまだない場合は作成します。
    /// </summary>
    /// <returns>Clipが設定されていない場合 false</returns>
    bool Play();
    /// <summary>
    /// 指定された名前のStateを有効にして再生します。
    /// </summary>
    /// <param name="name">Stateの名前</param>
    /// <returns>Stateが見つからない場合 false</returns>
    bool Play(const std::string &name);
    /// <summary>
    /// すべてのStateを停止し、すべてのTransformを初期姿勢に戻します。
    /// </summary>
    void Stop();

    /// <summary>
    /// RootMotion用に蓄積されたRootBoneの移動量を取得します。Sampleの最後に 0 にリセットされます。
    /// </summary>
    [[nodiscard]] Vector3 GetDeltaPosition() const;
    /// <summary>
    /// RootMotion用に蓄積されたRootBoneの回転量を取得します。Sampleの最後にリセットされます。
    /// </summary>
    Quaternion GetDeltaRotation() const;

    /// <summary>
    /// ClipからAnimationStateを作成し、指定された名前で登録します。
    /// </summary>
    /// <param name="clip">再生するClip</param>
    /// <param name="name">Stateの名前</param>
    /// <returns>登録されたStateのIteratorと、新しく追加された場合 true</returns>
    std::pair<StateIterator, bool> AddClip(const std::shared_ptr<AnimationClip> &clip, const std::string &name);
    /// <summary>
    /// AnimationStateを指定された名前で登録します。同じ名前のStateがある場合は上書きします。
    /// </summary>
    /// <param name="state">登録するState</param>
    /// <param name="name">Stateの名前</param>
    /// <returns>登録されたStateのIteratorと、新しく追加された場合 true</returns>
    std::pair<StateIterator, bool> AddState(std::shared_ptr<AnimationState> state, const std::string &name);
    /// <summary>
    /// 名前からAnimationStateを取得します。
    /// </summary>
    /// <param name="name">Stateの名前</param>
    /// <returns>AnimationState。見つからない場合 nullptr</returns>
    std::shared_ptr<AnimationState> FindClip(const std::string &name) const;
    /// <summary>
    /// 指定された名前のStateを取り除きます。
    /// </summary>
    /// <param name="name">Stateの名前</param>
    void RemoveClip(const std::string &name);
    /// <summary>
    /// 登録されているStateの数を取得します。
    /// </summary>
    [[nodiscard]] size_t ClipCount() const;

    /// <summary>
    /// 有効なすべてのStateの時間を進め、weightに応じてブレンドした姿勢を各Transformに適用します。RootMotionが有効な場合は、RootBoneの移動と回転を自身のTransformに適用します。
    /// </summary>
    void Sample();

    /// <summary>
    /// 再生中であるかどうかを取得します。
    /// </summary>
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
                CEREAL_NVP(m_apply_root_motion_),
                CEREAL_NVP(m_root_bone_)
            );
        }
    }
};
}

CEREAL_CLASS_VERSION(engine::AnimationComponent, 2)