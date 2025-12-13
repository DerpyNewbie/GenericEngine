#pragma once
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

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(position_key),
            CEREAL_NVP(scale_key),
            CEREAL_NVP(rotation_key),
            CEREAL_NVP(position_index),
            CEREAL_NVP(scale_index),
            CEREAL_NVP(rotation_index)
        );
    }

};

class AnimationClip : public Object, public Inspectable
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

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            cereal::base_class<Object>(this),
            CEREAL_NVP(m_curves_),
            CEREAL_NVP(m_length_),
            CEREAL_NVP(m_frame_rate_)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::TransformAnimationCurve, 1)

CEREAL_CLASS_VERSION(engine::AnimationClip, 1)