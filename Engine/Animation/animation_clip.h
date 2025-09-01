#pragma once
#include "Asset/inspectable_asset.h"

#include <chrono>

namespace engine
{
class GameObject;

struct CompareByFloat
{
    bool operator()(const std::pair<float, Quaternion> &lhs,
                    const std::pair<float, Quaternion> &rhs) const
    {
        return lhs.first < rhs.first;
    }

    bool operator()(const std::pair<float, Vector3> &lhs,
                    const std::pair<float, Vector3> &rhs) const
    {
        return lhs.first < rhs.first;
    }
};

struct TransformAnimationCurve
{
    std::set<std::pair<float, Vector3>, CompareByFloat> position_key;
    std::set<std::pair<float, Vector3>, CompareByFloat> scale_key;
    std::set<std::pair<float, Quaternion>, CompareByFloat> rotation_key;

    Matrix Evaluate(float time);
};

class AnimationClip : public InspectableAsset
{
    friend class FbxImporter;
    std::unordered_map<std::string, TransformAnimationCurve> m_curves_;
    float m_length_ = 0;
    float m_frame_rate_ = 0;

public:
    void OnInspectorGui() override;

    std::vector<std::string> GetNodeNames();
    Matrix GetSampledMatrix(float time, const std::string &path);

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