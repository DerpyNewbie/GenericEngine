#pragma once

#include <DxLib.h>
#include <type_traits>

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

inline VECTOR operator*(const VECTOR &v, const MATRIX &m)
{
    return VTransform(v, m);
}

inline MATRIX operator*(const MATRIX &a, const MATRIX &b)
{
    return MMult(a, b);
}

inline MATRIX_D operator*(const MATRIX_D &a, const MATRIX_D &b)
{
    return MMultD(a, b);
}