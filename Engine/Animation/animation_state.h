#pragma once
#include "animation_clip.h"
#include "Asset/asset_ptr.h"

namespace engine
{
enum class kWrapMode
{
    kOnce,
    kLoop,
    kPingPong
};

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
    void SetClip(std::shared_ptr<AnimationClip> clip);
    void UpdateTime();
    [[nodiscard]] float GetTime() const;
    [[nodiscard]] float NormalizedTime() const;
    [[nodiscard]] float NormalizedSpeed() const;
    [[nodiscard]] bool HasEnded() const;
    void SetNormalizedTime(float normalized_time);
    void SetNormalizedSpeed(float normalized_speed);
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