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

    static Quaternion FromMatrix(const Matrix4x4 &matrix);

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
    Vector3 operator*(const Vector3 &v) const;

    Quaternion operator*(const Quaternion &other) const;
};

constexpr Quaternion Quaternion::Identity()
{
    return Quaternion{0, 0, 0, 1};
}
}