#pragma once
#include "quaternion.h"
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

    constexpr static Matrix4x4 FromScale(const Vector3 &scale);

    constexpr static Matrix4x4 FromTranslate(const Vector3 &translate);

    constexpr static Matrix4x4 FromRotation(const Quaternion &rotation);

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
            mat.m[y][x] = y == x ? 1 : 0;
    return mat;
}
constexpr Matrix4x4 Matrix4x4::FromScale(const Vector3 &scale)
{
    Matrix4x4 mat;
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            mat.m[y][x] = y == x ? scale[y] : 0;
    return mat;
}
constexpr Matrix4x4 Matrix4x4::FromTranslate(const Vector3 &translate)
{
    Matrix4x4 mat;
    for (int y = 0; y < 3; ++y)
        mat.m[3][y] = translate[y];
    return mat;
}
constexpr Matrix4x4 Matrix4x4::FromRotation(const Quaternion &rotation)
{
    Matrix4x4 mat;
    const float q0 = rotation.x, q1 = rotation.y, q2 = rotation.z, q3 = rotation.w;

    mat.m[0][0] = 2 * (q0 * q0 + q1 * q1) - 1;
    mat.m[0][1] = 2 * (q1 * q2 - q0 * q3);
    mat.m[0][2] = 2 * (q1 * q3 + q0 * q2);
    mat.m[0][3] = 0;

    mat.m[1][0] = 2 * (q1 * q2 + q0 * q3);
    mat.m[1][1] = 2 * (q0 * q0 + q2 * q2) - 1;
    mat.m[1][2] = 2 * (q2 * q3 - q0 * q1);
    mat.m[1][3] = 0;

    mat.m[2][0] = 2 * (q1 * q3 - q0 * q2);
    mat.m[2][1] = 2 * (q2 * q3 + q0 * q1);
    mat.m[2][2] = 2 * (q0 * q0 + q3 * q3) - 1;
    mat.m[2][3] = 0;

    mat.m[3][0] = 0;
    mat.m[3][1] = 0;
    mat.m[3][2] = 0;
    mat.m[3][3] = 1;

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