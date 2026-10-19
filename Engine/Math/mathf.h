#pragma once

namespace engine
{
/// <summary>
/// floatの計算に関する定数と便利関数をまとめたクラスです。
/// </summary>
class Mathf
{
public:
    constexpr static float kPi = std::numbers::pi_v<float>;
    constexpr static float kDeg2Rad = kPi / 180.0F;
    constexpr static float kRad2Deg = 180.0F / kPi;
    constexpr static float kEpsilon = 1e-15f;
    constexpr static char kDefaultFloatFormat[] = "{:1.2f}";

    /// <summary>
    /// 2つの値のうち大きい方を返します。
    /// </summary>
    static float Max(const float lhs, const float rhs)
    {
        return lhs < rhs ? rhs : lhs;
    }

    /// <summary>
    /// 2つの値のうち小さい方を返します。
    /// </summary>
    static float Min(const float lhs, const float rhs)
    {
        return lhs < rhs ? lhs : rhs;
    }

    /// <summary>
    /// 値を v_min ～ v_max の範囲に制限します。
    /// </summary>
    static float Clamp(float value, const float v_min, const float v_max)
    {
        if (value < v_min)
            value = v_min;
        else if (value > v_max)
            value = v_max;
        return value;
    }

    /// <summary>
    /// 値を 0 ～ 1 の範囲に制限します。
    /// </summary>
    static float Clamp01(const float value)
    {
        return Clamp(value, 0, 1);
    }

    /// <summary>
    /// 値が v_min 以上 v_max 以下であるかどうかを判定します。
    /// </summary>
    static bool InRange(const float value, const float v_min, const float v_max)
    {
        return v_min <= value && value <= v_max;
    }

    /// <summary>
    /// 値の符号を返します。
    /// </summary>
    /// <returns>正の場合 1、負の場合 -1、0 の場合 0</returns>
    static float Sign(const float value)
    {
        if (value > 0)
            return 1.0F;
        if (value < 0)
            return -1.0F;
        return 0;
    }

    /// <summary>
    /// 2つの値の差がkEpsilon未満であるかどうかを判定します。
    /// </summary>
    static bool Approximately(const float lhs, const float rhs)
    {
        return std::abs(lhs - rhs) < kEpsilon;
    }

    /// <summary>
    /// a から b へ線形補間します。
    /// </summary>
    /// <param name="t">補間の割合。0 で a、1 で b</param>
    template <typename T>
    static T Lerp(const T &a, const T &b, const float t)
    {
        return a + (b - a) * t;
    }

    /// <summary>
    /// a から b へ球面線形補間します。回転量が小さくなる向きで補間します。
    /// </summary>
    /// <param name="t">補間の割合。0 で a、1 で b</param>
    static DirectX::SimpleMath::Quaternion Slerp(const DirectX::SimpleMath::Quaternion &a, const DirectX::SimpleMath::Quaternion &b, const float t)
    {
        float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        dot = Clamp(dot, -1.0f, 1.0f);
        
        DirectX::SimpleMath::Quaternion end = b;
        if (dot < 0.0f)
        {
            dot = -dot;
            end = DirectX::SimpleMath::Quaternion(-b.x, -b.y, -b.z, -b.w);
        }

        if (Approximately(dot, 1.0F))
        {
            auto result = DirectX::SimpleMath::Quaternion(
                a.x + t * (end.x - a.x),
                a.y + t * (end.y - a.y),
                a.z + t * (end.z - a.z),
                a.w + t * (end.w - a.w)
                );
            result.Normalize();
            return result;
        }

        const float theta_0 = acosf(dot);
        const float theta = theta_0 * t;

        const float sin_theta = sinf(theta);
        const float sin_theta_0 = sinf(theta_0);

        const float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;
        const float s1 = sin_theta / sin_theta_0;

        const auto result = DirectX::SimpleMath::Quaternion(
            (a.x * s0) + (end.x * s1),
            (a.y * s0) + (end.y * s1),
            (a.z * s0) + (end.z * s1),
            (a.w * s0) + (end.w * s1)
            );
        return result;
    }

    /// <summary>
    /// ProjectionMatrixからNearとFarの距離を計算します。右手系の透視投影のMatrixを想定しています。
    /// </summary>
    /// <param name="proj">ProjectionMatrix</param>
    /// <param name="_near">計算されたNear</param>
    /// <param name="_far">計算されたFar</param>
    static void NearFar(const DirectX::SimpleMath::Matrix &proj, float &_near, float &_far)
    {
        const float a = proj.m[2][2];
        const float b = proj.m[3][2];

        _near = b / a;
        _far = b / (1.0f + a);
    }
};
}