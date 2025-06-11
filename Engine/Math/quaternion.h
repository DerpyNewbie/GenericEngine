#pragma once

#include <DxLib.h>

namespace engine
{
struct Vector3;

struct Matrix4x4;

struct Quaternion
{
    float x = 0, y = 0, z = 0, w = 0;

    constexpr Quaternion() = default;

    constexpr Quaternion(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w)
    {}

    static constexpr Quaternion Identity();

    static Quaternion FromMatrix(const MATRIX &matrix);

    static Quaternion FromEulerRadians(const Vector3 &euler);

    static Quaternion FromEulerDegrees(const Vector3 &euler);

    [[nodiscard]] Vector3 ToEulerRadians() const;

    [[nodiscard]] Vector3 ToEulerDegrees() const;

    [[nodiscard]] Matrix4x4 ToMatrix() const;

    [[nodiscard]] Quaternion Conjugate() const;

    [[nodiscard]] float Magnitude() const;

    [[nodiscard]] Quaternion Inverse() const;

    [[nodiscard]] Quaternion Normalized() const;

    void Normalize();

    Quaternion operator*(const Quaternion &other) const
    {
        return {
            w * other.x + x * other.w + y * other.z - z * other.y, // x
            w * other.y - x * other.z + y * other.w + z * other.x, // y
            w * other.z + x * other.y - y * other.x + z * other.w, // z
            w * other.w - x * other.x - y * other.y - z * other.z // w
        };
    }
};

constexpr Quaternion Quaternion::Identity()
{
    return Quaternion{0, 0, 0, 1};
}
}