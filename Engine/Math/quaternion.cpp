#include "quaternion.h"
#include "vector3.h"
#include "matrix4x4.h"
#include <cmath>

namespace engine
{
Quaternion Quaternion::FromMatrix(const Matrix4x4 &matrix)
{
    const float m00 = matrix.m[0][0], m01 = matrix.m[0][1], m02 = matrix.m[0][2];
    const float m10 = matrix.m[1][0], m11 = matrix.m[1][1], m12 = matrix.m[1][2];
    const float m20 = matrix.m[2][0], m21 = matrix.m[2][1], m22 = matrix.m[2][2];

    // Using a more stable algorithm for matrix-to-quaternion conversion
    const float trace = m00 + m11 + m22;
    Quaternion q;

    if (trace > 0.0f)
    {
        const float s = sqrtf(trace + 1.0f) * 2.0f;
        q.w = 0.25f * s;
        q.x = (m21 - m12) / s;
        q.y = (m02 - m20) / s;
        q.z = (m10 - m01) / s;
    }
    else if ((m00 > m11) && (m00 > m22))
    {
        const float s = sqrtf(1.0f + m00 - m11 - m22) * 2.0f;
        q.w = (m21 - m12) / s;
        q.x = 0.25f * s;
        q.y = (m01 + m10) / s;
        q.z = (m02 + m20) / s;
    }
    else if (m11 > m22)
    {
        const float s = sqrtf(1.0f + m11 - m00 - m22) * 2.0f;
        q.w = (m02 - m20) / s;
        q.x = (m01 + m10) / s;
        q.y = 0.25f * s;
        q.z = (m12 + m21) / s;
    }
    else
    {
        const float s = sqrtf(1.0f + m22 - m00 - m11) * 2.0f;
        q.w = (m10 - m01) / s;
        q.x = (m02 + m20) / s;
        q.y = (m12 + m21) / s;
        q.z = 0.25f * s;
    }

    return q.Normalized();
}
Quaternion Quaternion::FromEulerRadians(const Vector3 &euler)
{
    const float pitch = euler.x * 0.5f; // X rotation
    const float yaw = euler.y * 0.5f; // Y rotation
    const float roll = euler.z * 0.5f; // Z rotation

    // Pre-calculate sines and cosines
    const float cp = cosf(pitch);
    const float sp = sinf(pitch);
    const float cy = cosf(yaw);
    const float sy = sinf(yaw);
    const float cr = cosf(roll);
    const float sr = sinf(roll);

    // ZYX order for left-handed system
    return Quaternion{
        cr * sp * cy - sr * cp * sy, // x
        cr * cp * sy + sr * sp * cy, // y
        sr * cp * cy + cr * sp * sy, // z
        cr * cp * cy - sr * sp * sy // w
    };
}
Quaternion Quaternion::FromEulerDegrees(const Vector3 &euler)
{
    return FromEulerRadians(euler * (DX_PI_F / 180.0f));
}
Vector3 Quaternion::ToEulerRadians() const
{
    Vector3 euler;

    // Pitch (x-axis rotation)
    const float sin_p = 2.0f * (w * x + y * z);
    const float cos_p = 1.0f - 2.0f * (x * x + y * y);
    euler.x = atan2f(sin_p, cos_p);

    // Yaw (y-axis rotation)
    const float sin_y = 2.0f * (w * y - z * x);
    if (abs(sin_y) >= 1.0f)
    {
        // Use 90 degrees if out of range
        euler.y = copysignf(DX_PI_F / 2.0f, sin_y);
    }
    else
    {
        euler.y = asinf(sin_y);
    }

    // Roll (z-axis rotation)
    const float sin_r = 2.0f * (w * z + x * y);
    const float cos_r = 1.0f - 2.0f * (y * y + z * z);
    euler.z = atan2f(sin_r, cos_r);

    return euler;
}
Vector3 Quaternion::ToEulerDegrees() const
{
    return ToEulerRadians() * (180.0f / DX_PI_F);
}
Matrix4x4 Quaternion::ToMatrix() const
{
    Matrix4x4 result;

    // For left-handed coordinate system
    result.m[0][0] = 1 - 2 * (y * y + z * z);
    result.m[0][1] = 2 * (x * y - w * z);
    result.m[0][2] = 2 * (x * z + w * y);
    result.m[0][3] = 0.0f;

    result.m[1][0] = 2 * (x * y + w * z);
    result.m[1][1] = 1 - 2 * (x * x + z * z);
    result.m[1][2] = 2 * (y * z - w * x);
    result.m[1][3] = 0.0f;

    result.m[2][0] = 2 * (x * z - w * y);
    result.m[2][1] = 2 * (y * z + w * x);
    result.m[2][2] = 1 - 2 * (x * x + y * y);
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}
Quaternion Quaternion::Conjugate() const
{
    return Quaternion{-x, -y, -z, w};
}
float Quaternion::Magnitude() const
{
    return sqrt(x * x + y * y + z * z + w * w);
}
Quaternion Quaternion::Inverse() const
{
    // For unit quaternions (normalized), inverse equals conjugate
    // For non-unit quaternions, we need to divide by the magnitude squared
    const float magnitude_sq = x * x + y * y + z * z + w * w;

    // Check if the quaternion is not zero
    if (magnitude_sq < 1e-6f)
        return Identity(); // Return identity if the quaternion is too close to zero

    const float inv_magnitude_sq = 1.0f / magnitude_sq;
    return {
        -x * inv_magnitude_sq,
        -y * inv_magnitude_sq,
        -z * inv_magnitude_sq,
        w * inv_magnitude_sq
    };
}
Quaternion Quaternion::Normalized() const
{
    const float mag = Magnitude();
    if (mag < 1e-6f)
        return Identity();

    const float inv_mag = 1.0f / mag;
    return {
        x * inv_mag,
        y * inv_mag,
        z * inv_mag,
        w * inv_mag
    };
}
void Quaternion::Normalize()
{
    const float mag = Magnitude();
    if (mag < 1e-6f)
    {
        *this = Identity();
        return;
    }

    const float inv_mag = 1.0f / mag;
    x *= inv_mag;
    y *= inv_mag;
    z *= inv_mag;
    w *= inv_mag;
}
Quaternion Quaternion::operator*(const Quaternion &other) const
{
    return {
        w * other.x + x * other.w - y * other.z + z * other.y, // x
        w * other.y + x * other.z + y * other.w - z * other.x, // y
        w * other.z - x * other.y + y * other.x + z * other.w, // z
        w * other.w - x * other.x - y * other.y - z * other.z // w
    };
}
Vector3 Quaternion::operator*(const Vector3 &v) const
{
    // For the left-handed coordinate system, we need to adjust the cross-product signs
    const Quaternion p(v.x, v.y, v.z, 0);

    // For left-handed system: q * p * q^(-1)
    const auto q_conj = Conjugate();
    const auto result = (*this * p) * q_conj;

    return {result.x, result.y, result.z};
}
}