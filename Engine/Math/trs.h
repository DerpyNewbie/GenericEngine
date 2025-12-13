#pragma once

namespace engine
{
struct TRS
{
    Vector3 translation;
    Vector3 scale;
    Quaternion rotation;

    TRS() = default;
    explicit TRS(Matrix matrix);

    [[nodiscard]] Matrix GetMatrix() const;

    static TRS Blend(const TRS &from, const TRS &to, float t);

    template <class Archive>
    void serialize(Archive &ar)
    {
        ar(CEREAL_NVP(translation), CEREAL_NVP(scale), CEREAL_NVP(rotation));
    }
};
}