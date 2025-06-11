#pragma once

#include <numbers>

namespace engine
{
class Mathf
{
public:
    constexpr static float kPi = std::numbers::pi;
    constexpr static float kDeg2Rad = kPi / 180;
    constexpr static float kRad2Deg = 180 / kPi;
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
};
}