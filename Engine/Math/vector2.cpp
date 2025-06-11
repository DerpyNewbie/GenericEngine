#include <cmath>
#include <format>
#include "vector2.h"
#include "vector3.h"
#include "mathf.h"

namespace engine
{
float Vector2::Magnitude() const
{
    return sqrtf(x * x + y * y);
}

float Vector2::SqrMagnitude() const
{
    return x * x + y * y;
}

Vector2 Vector2::Normalized() const
{
    const float mag = Magnitude();
    return Vector2{x / mag, y / mag};
}

Vector2 Vector2::Rounded() const
{
    return {std::round(x), std::round(y)};
}

void Vector2::Set(const float nx, const float ny)
{
    x = nx;
    y = ny;
}

void Vector2::SetRounded()
{
    Set(std::round(x), std::round(y));
}

Vector2 Vector2::Copy() const
{
    return {x, y};
}

Vector2 Vector2::Lerp(const Vector2 current, const Vector2 target, const float progress)
{
    return (current * (1.0F - progress) + (target * progress));
}

float Vector2::DotNormalized(const Vector2 lhs, const Vector2 rhs)
{
    if (const float denominator = std::sqrt(lhs.SqrMagnitude() * rhs.SqrMagnitude()); denominator < Mathf::kEpsilon)
        return 0.0F;

    const auto lhs_n = lhs.Normalized();
    const auto rhs_n = rhs.Normalized();
    return lhs_n.x * rhs_n.x + lhs_n.y * rhs_n.y;
}

float Vector2::Dot(const Vector2 lhs, const Vector2 rhs)
{
    if (const float denominator = std::sqrt(lhs.SqrMagnitude() * rhs.SqrMagnitude()); denominator < Mathf::kEpsilon)
        return 0.0F;

    return lhs.x * rhs.x + lhs.y * rhs.y;
}

Vector2 Vector2::Projection(const Vector2 from, const Vector2 to)
{
    return to * Dot(to, from);
}

float Vector2::Distance(const Vector2 &from, const Vector2 &to)
{
    return (to - from).Magnitude();
}

Vector2::operator Vector3() const
{
    return {x, y, 0.0F};
}

std::string Vector2::ToString() const
{
    return "(" +
           std::format(Mathf::kDefaultFloatFormat, x) + ", " +
           std::format(Mathf::kDefaultFloatFormat, y) +
           ")";
}

Vector2 Vector2::ClosestPointOnLine(const Vector2 from, const Vector2 to, const Vector2 point)
{
    const auto from_p = point - from;
    const auto from_to = to - from;
    return from + from_to * Mathf::Clamp01(Dot(from_p, from_to) / from_to.SqrMagnitude());
}

Vector2 Vector2::Clamp(const Vector2 value, const Vector2 v_min, const Vector2 v_max)
{
    return {Mathf::Clamp(value.x, v_min.x, v_max.x), Mathf::Clamp(value.y, v_min.y, v_max.y)};
}
}