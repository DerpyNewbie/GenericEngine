#pragma once
#include "Asset/inspectable_asset.h"

namespace engine
{
class GameObject;

struct TransformAnimationCurve
{
    std::vector<std::pair<float, Vector3>> position_key;
    std::vector<std::pair<float, Vector3>> scale_key;
    std::vector<std::pair<float, Quaternion>> rotation_key;

    size_t position_index = 0;
    size_t scale_index = 0;
    size_t rotation_index = 0;
};

class AnimationClip : public InspectableAsset
{
    friend class FbxImporter;
    std::unordered_map<std::string, TransformAnimationCurve> m_curves_;
    float m_length_ = 0;
    float m_frame_rate_ = 0;

public:
    void OnInspectorGui() override;

    void Initialize();
    TransformAnimationCurve *FindCurve(const std::string &path);

    /// <summary>
    /// Total duration of this animation clip
    /// </summary>
    /// <returns>total duration of this animation clip in seconds</returns>
    [[nodiscard]] float Length() const;

    /// <summary>
    /// key-frame sample rate
    /// </summary>
    /// <returns>key-frame sample rate</returns>
    [[nodiscard]] float FrameRate() const;
};
}