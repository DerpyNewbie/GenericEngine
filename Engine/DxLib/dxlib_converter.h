#pragma once

namespace DxLibConverter
{
using namespace DirectX::SimpleMath;

inline VECTOR From(const Vector3 vector)
{
    return {vector.x, vector.y, vector.z};
}

inline MATRIX From(const Matrix &matrix)
{
    return {matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
            matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
            matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
            matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]};
}

inline COLOR_U8 From(const Color color)
{
    return {static_cast<BYTE>(color.B() * 255),
            static_cast<BYTE>(color.G() * 255),
            static_cast<BYTE>(color.R() * 255),
            static_cast<BYTE>(color.A() * 255)};
}

inline unsigned int Argb(const Color color)
{
    auto [b, g, r, a] = From(color);
    return (a << 24) + (r << 16) + (g << 8) + b;
}

inline Vector2 To(const FLOAT2 f2)
{
    return {f2.u, f2.v};
}

inline Vector3 To(const VECTOR &vector)
{
    return {vector.x, vector.y, vector.z};
}

inline Matrix To(const MATRIX &matrix)
{
    return {matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
            matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
            matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
            matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]};
}

inline Color To(const COLOR_U8 color)
{
    return Color{color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};
}


};