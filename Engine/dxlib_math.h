#pragma once

#include <DxLib.h>
#include <type_traits>
#include <Math/quaternion.h>

template <class T>
constexpr bool kIsVector3 = false;

template <>
constexpr bool kIsVector3<VECTOR> = true;

template <>
constexpr bool kIsVector3<VECTOR_D> = true;

template <class T>
constexpr bool kIsVector3<std::is_layout_compatible<T, VECTOR>> = true;

template <class T>
constexpr bool kIsVector3<std::is_layout_compatible<T, VECTOR_D>> = true;

template <class T>
constexpr bool kIsMatrix = false;

template <>
constexpr bool kIsMatrix<MATRIX> = true;

template <>
constexpr bool kIsMatrix<MATRIX_D> = true;

template <class V> V operator+(const V &a, const V &b) requires (kIsVector3<V>)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

template <class V> V operator-(const V &a, const V &b) requires (kIsVector3<V>)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

template <class V> V operator*(const V &a, const float &b) requires (kIsVector3<V>)
{
    return {a.x * b, a.y * b, a.z * b};
}

inline VECTOR operator*(const VECTOR &v, const engine::Quaternion &q)
{
    const float x2 = q.x * 2.0f;
    const float y2 = q.y * 2.0f;
    const float z2 = q.z * 2.0f;
    const float xx2 = q.x * x2;
    const float yy2 = q.y * y2;
    const float zz2 = q.z * z2;
    const float xy2 = q.x * y2;
    const float xz2 = q.x * z2;
    const float yz2 = q.y * z2;
    const float wx2 = q.w * x2;
    const float wy2 = q.w * y2;
    const float wz2 = q.w * z2;

    return {
        v.x * (1.0f - yy2 - zz2) + v.y * (xy2 - wz2) + v.z * (xz2 + wy2),
        v.x * (xy2 + wz2) + v.y * (1.0f - xx2 - zz2) + v.z * (yz2 - wx2),
        v.x * (xz2 - wy2) + v.y * (yz2 + wx2) + v.z * (1.0f - xx2 - yy2)
    };
}

inline VECTOR operator*(const VECTOR &v, const MATRIX &m)
{
    return VTransform(v, m);
}