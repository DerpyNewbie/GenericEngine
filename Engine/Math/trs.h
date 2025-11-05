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
};
}