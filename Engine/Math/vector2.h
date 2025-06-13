#pragma once

#include <DxLib.h>
#include <stdexcept>
#include <string>

namespace engine
{
struct Vector3;

struct Vector2
{
    float x, y;

    constexpr Vector2() : Vector2(0, 0)
    {}

    constexpr Vector2(const float x, const float y) : x(x), y(y)
    {}

    constexpr Vector2(const int x, const int y) : x(static_cast<float>(x)), y(static_cast<float>(y))
    {}

    // ReSharper disable once CppNonExplicitConvertingConstructor
    constexpr Vector2(const FLOAT2 v) : x(v.u), y(v.v)
    {}

    /// @returns (0, 0)
    static constexpr Vector2 Zero()
    {
        return {0, 0};
    }

    /// @returns (1, 1)
    static constexpr Vector2 One()
    {
        return {1, 1};
    }

    /// @returns (0, 1)
    static constexpr Vector2 Up()
    {
        return {0, 1};
    }

    /// @returns (0, -1)
    static constexpr Vector2 Down()
    {
        return {0, -1};
    }

    /// @returns (-1, 0)
    static constexpr Vector2 Left()
    {
        return {-1, 0};
    }

    /// @returns (1, 0)
    static constexpr Vector2 Right()
    {
        return {1, 0};
    }

    /// @returns Strength of this vector.
    /// @remarks square root of Vector2::sqrMagnitude(). use Vector2::sqrMagnitude() whenever it's possible as this function is slower.
    [[nodiscard]] float Magnitude() const;

    /// @returns Strength of this vector (squared).
    [[nodiscard]] float SqrMagnitude() const;

    /// @returns Normalized vector.
    /// @remarks Original elements will be left untouched.
    [[nodiscard]] Vector2 Normalized() const;

    /// @returns Vector rounded to int.
    /// @remarks Original elements will be left untouched.
    [[nodiscard]] Vector2 Rounded() const;

    /// @brief Sets the element directly.
    void Set(float nx, float ny);

    /// @brief Rounds the element directly.
    void SetRounded();

    /// @returns Copy of this Vector.
    [[nodiscard]] Vector2 Copy() const;

    /// @returns Linear interpolated Vector from current to target by progress.
    static Vector2 Lerp(Vector2 current, Vector2 target, float progress);

    /// @returns Dot product of normalized Vectors.
    static float DotNormalized(Vector2 lhs, Vector2 rhs);

    /// @returns Dot product of two Vectors.
    static float Dot(Vector2 lhs, Vector2 rhs);

    /// @returns Projected Vector from lhs to rhs.
    static Vector2 Projection(Vector2 from, Vector2 to);

    /// @returns closest point over the line between "from" and "to" from "point"
    static Vector2 ClosestPointOnLine(Vector2 from, Vector2 to, Vector2 point);

    /// @returns Clamped Vector of value from v_min to v_max
    static Vector2 Clamp(Vector2 value, Vector2 v_min, Vector2 v_max);

    /// @returns Distance between lhs to rhs.
    static float Distance(const Vector2 &from, const Vector2 &to);

    /// @returns (x.xx, y.yy)
    [[nodiscard]] std::string ToString() const;

    constexpr Vector2 *operator+=(const Vector2 &v)
    {
        x += v.x;
        y += v.y;
        return this;
    }

    constexpr Vector2 *operator-=(const Vector2 &v)
    {
        x -= v.x;
        y -= v.y;
        return this;
    }

    constexpr Vector2 *operator*=(const Vector2 &v)
    {
        x *= v.x;
        y *= v.y;
        return this;
    }

    constexpr Vector2 *operator*=(const float &v)
    {
        x *= v;
        y *= v;
        return this;
    }

    constexpr Vector2 operator+() const
    {
        return Vector2{+x, +y};
    }

    constexpr Vector2 operator+(const Vector2 &v) const
    {
        return Vector2{x + v.x, y + v.y};
    }

    constexpr Vector2 operator-() const
    {
        return Vector2{-x, -y};
    }

    constexpr Vector2 operator-(const Vector2 &v) const
    {
        return Vector2{x - v.x, y - v.y};
    }

    constexpr Vector2 operator*(const Vector2 &v) const
    {
        return Vector2{x * v.x, y * v.y};
    }

    constexpr Vector2 operator*(const float &v) const
    {
        return Vector2{x * v, y * v};
    }

    constexpr bool operator<(const Vector2 &v) const
    {
        return x < v.x && y < v.y;
    }

    constexpr bool operator>(const Vector2 &v) const
    {
        return x > v.x && y > v.y;
    }

    constexpr float &operator[](const int &v)
    {
        switch (v)
        {
        case 0:
            return x;
        case 1:
            return y;
        default:
            throw std::out_of_range("Cannot access to above 1 or below 0");
        }
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator Vector3() const; // NOLINT(*-explicit-constructor)
};
}