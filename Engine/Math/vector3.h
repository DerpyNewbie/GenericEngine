#pragma once
#include <DxLib.h>

#include <string>
#include <stdexcept>

namespace engine
{
struct Vector2;

struct Quaternion;

struct Vector3
{
    float x, y, z;

    constexpr Vector3() : Vector3(0, 0, 0)
    {}

    constexpr Vector3(const float x, const float y) : Vector3(x, y, 0.0F)
    {}

    constexpr Vector3(const float x, const float y, const float z) : x(x), y(y), z(z)
    {}

    constexpr Vector3(const int x, const int y) : Vector3(static_cast<float>(x), static_cast<float>(y), 0.0F)
    {}

    constexpr Vector3(const int x, const int y, const int z) : Vector3(static_cast<float>(x),
                                                                       static_cast<float>(y),
                                                                       static_cast<float>(z))
    {}

    // ReSharper disable once CppNonExplicitConvertingConstructor
    constexpr Vector3(const VECTOR v) : Vector3(v.x, v.y, v.z)
    {}

    /// @returns (0, 0, 0)
    static constexpr Vector3 Zero()
    {
        return {0, 0, 0};
    }

    /// @returns (1, 1, 1)
    static constexpr Vector3 One()
    {
        return {1, 1, 1};
    };

    /// @returns (0, 1, 0)
    static constexpr Vector3 Up()
    {
        return {0, 1, 0};
    }

    /// @returns (0, -1, 0)
    static constexpr Vector3 Down()
    {
        return {0, -1, 0};
    }

    /// @returns (-1, 0, 0)
    static constexpr Vector3 Left()
    {
        return {-1, 0, 0};
    }

    /// @returns (1, 0, 0)
    static constexpr Vector3 Right()
    {
        return {1, 0, 0};
    }

    /// @returns (0, 0, 1)
    static constexpr Vector3 Forward()
    {
        return {0, 0, 1};
    }

    /// @returns (0, 0, -1)
    static constexpr Vector3 Back()
    {
        return {0, 0, -1};
    }

    /// @returns Strength of this vector.
    [[nodiscard]] float Magnitude() const
    {
        return sqrtf(x * x + y * y + z * z);
    }

    /// @returns Strength of this vector (squared).
    [[nodiscard]] float SqrMagnitude() const;

    /// @returns Normalized vector.
    /// @remarks Original elements will be left untouched.
    [[nodiscard]] Vector3 Normalized() const
    {
        const float mag = Magnitude();
        return {x / mag, y / mag, z / mag};
    }

    /// @returns Vector rounded to int.
    /// @remarks Original elements will be left untouched.
    [[nodiscard]] Vector3 Rounded() const;

    /// @brief Sets the element directly.
    void Set(float nx, float ny, float nz);

    /// @brief Rounds the element directly.
    void SetRounded();

    /// @returns Copy of this Vector.
    [[nodiscard]] Vector3 Copy() const;

    /// @returns Linear interpolated Vector from current to target by progress.
    static Vector3 Lerp(Vector3 current, Vector3 target, float progress);

    /// @returns Dot product of normalized Vectors.
    static float DotNormalized(Vector3 lhs, Vector3 rhs);

    /// @returns Dot product of two Vectors.
    static float Dot(Vector3 lhs, Vector3 rhs);

    /// @returns Projected Vector from lhs to rhs.
    static Vector3 Projection(Vector3 from, Vector3 to);

    /// @returns Clamped Vector from min to max.
    static Vector3 Limit(Vector3 v, Vector3 min, Vector3 max);

    /// @returns Distance between lhs to rhs.
    static float Distance(const Vector3 &from, const Vector3 &to);

    /// @returns (x.xx, y.yy, z.zz)
    [[nodiscard]] std::string ToString() const;

    constexpr Vector3 *operator+=(const Vector3 &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return this;
    }

    constexpr Vector3 *operator-=(const Vector3 &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return this;
    }

    constexpr Vector3 *operator*=(const Vector3 &v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return this;
    }

    constexpr Vector3 *operator*=(const float &v)
    {
        x *= v;
        y *= v;
        z *= v;
        return this;
    }

    constexpr Vector3 operator+() const
    {
        return {+x, +y, +z};
    }

    constexpr Vector3 operator+(const Vector3 &v) const
    {
        return {x + v.x, y + v.y, z + v.z};
    }

    constexpr Vector3 operator-() const
    {
        return {-x, -y, -z};
    }

    constexpr Vector3 operator-(const Vector3 &v) const
    {
        return {x - v.x, y - v.y, z - v.z};
    }

    constexpr Vector3 operator*(const Vector3 &v) const
    {
        return {x * v.x, y * v.y, z * v.z};
    }

    constexpr Vector3 operator*(const float &v) const
    {
        return {x * v, y * v, z * v};
    }

    constexpr bool operator<(const Vector3 &v) const
    {
        return x < v.x && y < v.y && z < v.z;
    }

    constexpr bool operator>(const Vector3 &v) const
    {
        return x > v.x && y > v.y && z > v.z;
    }

    constexpr float operator[](const int &v) const
    {
        switch (v)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            throw std::out_of_range("Cannot access to above 2 or below 0");
        }
    }

    constexpr float &operator[](const int &v)
    {
        switch (v)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            throw std::out_of_range("Cannot access to above 2 or below 0");
        }
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator Vector2() const; // NOLINT(*-explicit-constructor)
    static Vector3 Cross(Vector3 lhs, Vector3 rhs);

    // ReSharper disable once CppNonExplicitConversionOperator
    operator VECTOR() const
    {
        return {x, y, z};
    }

    Vector3 &operator=(const VECTOR &vec);

    Vector3 operator*(const MATRIX &m) const;
};
}