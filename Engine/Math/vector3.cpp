#include <format>
#include "quaternion.h"
#include "vector3.h"
#include "vector2.h"
#include "mathf.h"

namespace engine
{
float Vector3::SqrMagnitude() const
{
    return x * x + y * y + z * z;
}

Vector3 Vector3::Rounded() const
{
    return {static_cast<int>(x), static_cast<int>(y), static_cast<int>(z)};
}

void Vector3::Set(const float x, const float y, const float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Vector3::SetRounded()
{
    Set(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
}

Vector3 Vector3::Copy() const
{
    return {x, y, z};
}

Vector3 Vector3::Lerp(const Vector3 current, const Vector3 target, const float progress)
{
    return (current * (1.0F - progress) + (target * progress));
}

float Vector3::DotNormalized(const Vector3 lhs, const Vector3 rhs)
{
    float denominator = std::sqrt(lhs.SqrMagnitude() * rhs.SqrMagnitude());
    if (denominator < Mathf::kEpsilon)
        return 0.0F;

    auto lhs_n = lhs.Normalized();
    auto rhs_n = rhs.Normalized();
    return lhs_n.x * rhs_n.x + lhs_n.y * rhs_n.y + lhs_n.z * rhs_n.z;
}

float Vector3::Dot(const Vector3 lhs, const Vector3 rhs)
{
    float denominator = std::sqrt(lhs.SqrMagnitude() * rhs.SqrMagnitude());
    if (denominator < Mathf::kEpsilon)
        return 0.0F;

    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vector3 Vector3::Projection(const Vector3 from, const Vector3 to)
{
    return to * Dot(to, from);
}

Vector3 Vector3::Limit(const Vector3 v, const Vector3 min, const Vector3 max)
{
    return {
        std::fmin(std::fmax(v.x, min.x), max.x),
        std::fmin(std::fmax(v.y, min.y), max.y),
        std::fmin(std::fmax(v.z, min.z), max.z)
    };
}

float Vector3::Distance(const Vector3 &from, const Vector3 &to)
{
    return (to - from).Magnitude();
}

Vector3 Vector3::operator*(const Quaternion &v) const
{
    Quaternion q = {x, y, z, 0};
    auto q_conjugate = v.Inverse();
    auto q_result = (v * q) * q_conjugate;

    return {q_result.x, q_result.y, q_result.z};
}

Vector3::operator Vector2() const
{
    return {x, y};
}

std::string Vector3::ToString() const
{
    return "(" +
           std::format(Mathf::kDefaultFloatFormat, x) + ", " +
           std::format(Mathf::kDefaultFloatFormat, y) + ", " +
           std::format(Mathf::kDefaultFloatFormat, z) +
           ")";
}

Vector3 Vector3::Cross(const Vector3 lhs, const Vector3 rhs)
{
    return {lhs.y * rhs.z - rhs.y * lhs.z,
            lhs.z * rhs.x - rhs.z * lhs.x,
            lhs.x * rhs.y - rhs.x * lhs.y};
}
Vector3 &Vector3::operator=(const VECTOR &vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
    return *this;
}
}