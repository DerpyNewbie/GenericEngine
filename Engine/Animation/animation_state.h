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

struct AnimationState
{
    bool enabled = true;
    AssetPtr<AnimationClip> clip;
    std::string name;
    float speed = 1.0f;
    float time = 0.0f;
    float weight = 1.0f;
    float length = 0.0f;
    kWrapMode wrap_mode = kWrapMode::kOnce;

    [[nodiscard]] float NormalizedTime() const;
    [[nodiscard]] float NormalizedSpeed() const;

    void SetNormalizedTime(float normalized_time);
    void SetNormalizedSpeed(float normalized_speed);
    void Stop();
};
}