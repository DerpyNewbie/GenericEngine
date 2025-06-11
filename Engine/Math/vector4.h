#pragma once

namespace engine
{
struct Vector4
{
    float x, y, z, w;
    constexpr Vector4() : x(0), y(0), z(0), w(0)
    {}
    constexpr Vector4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w)
    {}

    // Vector multiplication (component-wise/Hadamard product)
    constexpr Vector4 operator*(const Vector4 &other) const
    {
        return {
            x * other.x,
            y * other.y,
            z * other.z,
            w * other.w
        };
    }
    // Vector addition
    constexpr Vector4 operator+(const Vector4 &other) const
    {
        return {
            x + other.x,
            y + other.y,
            z + other.z,
            w + other.w
        };
    }

    // Vector subtraction
    constexpr Vector4 operator-(const Vector4 &other) const
    {
        return {
            x - other.x,
            y - other.y,
            z - other.z,
            w - other.w
        };
    }

    // Scalar multiplication
    constexpr Vector4 operator*(float scalar) const
    {
        return {
            x * scalar,
            y * scalar,
            z * scalar,
            w * scalar
        };
    }
};
}