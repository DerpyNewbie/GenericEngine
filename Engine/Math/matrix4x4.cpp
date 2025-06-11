#include "matrix4x4.h"

#include "vector4.h"

namespace engine
{
Matrix4x4 Matrix4x4::Inverse() const
{
    Matrix4x4 result;

    // Calculate the cofactors
    float coef00 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
    float coef02 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
    float coef03 = m[2][1] * m[3][2] - m[2][2] * m[3][1];

    float coef04 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
    float coef06 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
    float coef07 = m[1][1] * m[3][2] - m[1][2] * m[3][1];

    float coef08 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
    float coef10 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
    float coef11 = m[1][1] * m[2][2] - m[1][2] * m[2][1];

    float coef12 = m[0][2] * m[3][3] - m[0][3] * m[3][2];
    float coef14 = m[0][1] * m[3][3] - m[0][3] * m[3][1];
    float coef15 = m[0][1] * m[3][2] - m[0][2] * m[3][1];

    float coef16 = m[0][2] * m[2][3] - m[0][3] * m[2][2];
    float coef18 = m[0][1] * m[2][3] - m[0][3] * m[2][1];
    float coef19 = m[0][1] * m[2][2] - m[0][2] * m[2][1];

    float coef20 = m[0][2] * m[1][3] - m[0][3] * m[1][2];
    float coef22 = m[0][1] * m[1][3] - m[0][3] * m[1][1];
    float coef23 = m[0][1] * m[1][2] - m[0][2] * m[1][1];

    Vector4 fac0(coef00, coef00, coef02, coef03);
    Vector4 fac1(coef04, coef04, coef06, coef07);
    Vector4 fac2(coef08, coef08, coef10, coef11);
    Vector4 fac3(coef12, coef12, coef14, coef15);
    Vector4 fac4(coef16, coef16, coef18, coef19);
    Vector4 fac5(coef20, coef20, coef22, coef23);

    Vector4 vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
    Vector4 vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
    Vector4 vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
    Vector4 vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

    Vector4 inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
    Vector4 inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
    Vector4 inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
    Vector4 inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

    Vector4 signA(+1, -1, +1, -1);
    Vector4 signB(-1, +1, -1, +1);

    // Calculate inverse matrix
    Matrix4x4 inverse;
    inverse.m[0][0] = inv0.x * signA.x;
    inverse.m[0][1] = inv0.y * signA.y;
    inverse.m[0][2] = inv0.z * signA.z;
    inverse.m[0][3] = inv0.w * signA.w;

    inverse.m[1][0] = inv1.x * signB.x;
    inverse.m[1][1] = inv1.y * signB.y;
    inverse.m[1][2] = inv1.z * signB.z;
    inverse.m[1][3] = inv1.w * signB.w;

    inverse.m[2][0] = inv2.x * signA.x;
    inverse.m[2][1] = inv2.y * signA.y;
    inverse.m[2][2] = inv2.z * signA.z;
    inverse.m[2][3] = inv2.w * signA.w;

    inverse.m[3][0] = inv3.x * signB.x;
    inverse.m[3][1] = inv3.y * signB.y;
    inverse.m[3][2] = inv3.z * signB.z;
    inverse.m[3][3] = inv3.w * signB.w;

    // Calculate determinant
    float det = m[0][0] * inverse.m[0][0] +
                m[0][1] * inverse.m[1][0] +
                m[0][2] * inverse.m[2][0] +
                m[0][3] * inverse.m[3][0];

    if (std::abs(det) < 1e-6f)
    {
        throw std::runtime_error("Matrix is not invertible");
    }

    // Divide by determinant
    det = 1.0f / det;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            inverse.m[i][j] *= det;
        }
    }

    return inverse;
}
Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &other) const
{
    Matrix4x4 mat;
    for (int y = 0; y < 4; ++y)
        for (int x = 0; x < 4; ++x)
            mat.m[y][x] = m[y][0] * other.m[0][x] +
                          m[y][1] * other.m[1][x] +
                          m[y][2] * other.m[2][x] +
                          m[y][3] * other.m[3][x];
    return mat;
}
Matrix4x4::operator tagMATRIX() const
{
    // ReSharper disable once CppCStyleCast
    return *(tagMATRIX *)this;
}
}