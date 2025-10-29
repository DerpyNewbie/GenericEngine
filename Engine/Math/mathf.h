#pragma once

namespace engine
{
class Mathf
{
public:
    constexpr static float kPi = std::numbers::pi_v<float>;
    constexpr static float kDeg2Rad = kPi / 180.0F;
    constexpr static float kRad2Deg = 180.0F / kPi;
    constexpr static float kEpsilon = 1e-15f;
    constexpr static char kDefaultFloatFormat[] = "{:1.2f}";

    static float Max(const float lhs, const float rhs)
    {
        return lhs < rhs ? rhs : lhs;
    }

    static float Min(const float lhs, const float rhs)
    {
        return lhs < rhs ? lhs : rhs;
    }

    static float Clamp(float value, const float v_min, const float v_max)
    {
        if (value < v_min)
            value = v_min;
        else if (value > v_max)
            value = v_max;
        return value;
    }

    static float Clamp01(const float value)
    {
        return Clamp(value, 0, 1);
    }

    static bool InRange(const float value, const float v_min, const float v_max)
    {
        return v_min <= value && value <= v_max;
    }

    static float Sign(const float value)
    {
        if (value > 0)
            return 1.0F;
        if (value < 0)
            return -1.0F;
        return 0;
    }

    static bool Approximately(const float lhs, const float rhs)
    {
        return std::abs(lhs - rhs) < kEpsilon;
    }
    static DirectX::SimpleMath::Quaternion Slerp(const DirectX::SimpleMath::Quaternion &a, const DirectX::SimpleMath::Quaternion &b, const float t)
    {
        float dot = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;

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
};
}