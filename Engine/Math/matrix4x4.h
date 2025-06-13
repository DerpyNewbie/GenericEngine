#pragma once
#include "vector3.h"

namespace engine
{
// ReSharper disable once CppInconsistentNaming
struct Matrix4x4
{
    union
    {
        float m[4][4];
        unsigned long long mem_row[8];
    };

    constexpr Matrix4x4();

    // ReSharper disable once CppNonExplicitConvertingConstructor
    constexpr Matrix4x4(const MATRIX &other);

    constexpr static Matrix4x4 Identity();

    [[nodiscard]] constexpr Vector3 TransformPoint(Vector3 point) const;

    [[nodiscard]] constexpr Matrix4x4 Transpose() const;

    [[nodiscard]] Matrix4x4 Inverse() const;

    Matrix4x4 operator*(const Matrix4x4 &other) const;

    // ReSharper disable once CppNonExplicitConversionOperator
    operator MATRIX() const;
};

constexpr Matrix4x4::Matrix4x4()
{
    for (auto &y : m)
        for (float &x : y)
            x = 0;
}
constexpr Matrix4x4::Matrix4x4(const MATRIX &other)
{
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            m[y][x] = other.m[y][x];
}
constexpr Matrix4x4 Matrix4x4::Identity()
{
    Matrix4x4 mat;
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            mat.m[y][x] = y == x ? 1.0F : 0.0F;
    return mat;
}
constexpr Vector3 Matrix4x4::TransformPoint(Vector3 point) const
{
    for (int y = 0; y < 4; ++y)
    {
        float sum = 0;
        for (int x = 0; x < 4; ++x)
            sum += m[y][x] * point[x];
        point[y] = sum;
    }
    return point;
}
constexpr Matrix4x4 Matrix4x4::Transpose() const
{
    Matrix4x4 mat;
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            mat.m[y][x] = m[x][y];
    return mat;
}

}