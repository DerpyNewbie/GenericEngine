#pragma once
#include "animation_clip.h"
#include "Asset/asset_ptr.h"

namespace engine
{
/// <summary>
/// Animationが終端に達した時の動作です。
/// </summary>
enum class kWrapMode
{
    kOnce,
    kLoop,
    kPingPong
};

/// <summary>
/// 再生中のAnimationの状態(再生位置、速度、Weightなど)です。
/// </summary>
struct AnimationState final : Inspectable
{
    bool enabled = true;
    AssetPtr<AnimationClip> clip;
    std::string name;
    float speed = 1.0f;
    float time = 0.0f;
    float weight = 1.0f;
    float length = 0.0f;
    bool just_looped = false;
    kWrapMode wrap_mode = kWrapMode::kOnce;

    void OnInspectorGui() override;
    /// <summary>
    /// Clipを設定し、lengthをClipの長さに合わせます。
    /// </summary>
    /// <param name="clip">再生するClip</param>
    void SetClip(std::shared_ptr<AnimationClip> clip);
    /// <summary>
    /// DeltaTimeとspeedに応じて再生時間を進めます。kLoopで終端を超えた場合は先頭に戻します。
    /// </summary>
    void UpdateTime();
    /// <summary>
    /// 現在の再生位置(秒)を取得します。kPingPongの場合は折り返した位置を返します。
    /// </summary>
    [[nodiscard]] float GetTime() const;
    /// <summary>
    /// 再生時間をClipの長さで割った値を取得します。
    /// </summary>
    [[nodiscard]] float NormalizedTime() const;
    /// <summary>
    /// speedをClipの長さで割った値を取得します。
    /// </summary>
    [[nodiscard]] float NormalizedSpeed() const;
    /// <summary>
    /// kOnceのStateが最後まで再生されたかどうかを取得します。
    /// </summary>
    [[nodiscard]] bool HasEnded() const;
    /// <summary>
    /// Clipの長さに対する割合で再生時間を設定します。
    /// </summary>
    /// <param name="normalized_time">0 が先頭、1 が終端</param>
    void SetNormalizedTime(float normalized_time);
    /// <summary>
    /// Clipの長さに対する割合でspeedを設定します。
    /// </summary>
    /// <param name="normalized_speed">Clipの長さに掛ける値</param>
    void SetNormalizedSpeed(float normalized_speed);
    /// <summary>
    /// Stateを無効にし、再生時間を 0 に戻します。
    /// </summary>
    void Stop();

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(enabled),
            CEREAL_NVP(clip),
            CEREAL_NVP(name),
            CEREAL_NVP(speed),
            CEREAL_NVP(time),
            CEREAL_NVP(weight),
            CEREAL_NVP(length),
            CEREAL_NVP(wrap_mode)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::AnimationState, 1)