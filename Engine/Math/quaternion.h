#pragma once
#include <DxLib.h>
#include <cmath>
#include "dxlib_math.h"

struct Quaternion
{
    float x = 0, y = 0, z = 0, w = 0;

    constexpr Quaternion() = default;

    constexpr Quaternion(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w)
    {}

    static constexpr Quaternion Identity()
    {
        return Quaternion{0, 0, 0, 1};
    }

    static Quaternion FromMatrix(const MATRIX &matrix)
    {
        // Implementation based on Shepperd's method which is considered more robust
        // Source: https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf

        float m00 = matrix.m[0][0], m01 = matrix.m[0][1], m02 = matrix.m[0][2];
        float m10 = matrix.m[1][0], m11 = matrix.m[1][1], m12 = matrix.m[1][2];
        float m20 = matrix.m[2][0], m21 = matrix.m[2][1], m22 = matrix.m[2][2];

        float t;
        Quaternion q;

        // Check which diagonal element is largest
        if (m22 < 0)
        {
            if (m00 > m11)
            {
                // m00 is largest diagonal
                t = 1.0f + m00 - m11 - m22;
                q = Quaternion(t, m01 + m10, m20 + m02, m12 - m21);
            }
            else
            {
                // m11 is largest diagonal
                t = 1.0f - m00 + m11 - m22;
                q = Quaternion(m01 + m10, t, m12 + m21, m20 - m02);
            }
        }
        else
        {
            if (m00 < -m11)
            {
                // m22 is largest diagonal
                t = 1.0f - m00 - m11 + m22;
                q = Quaternion(m20 + m02, m12 + m21, t, m01 - m10);
            }
            else
            {
                // m00 + m11 + m22 is largest
                t = 1.0f + m00 + m11 + m22;
                q = Quaternion(m12 - m21, m20 - m02, m01 - m10, t);
            }
        }

        // Normalize and scale to get a unit quaternion
        const float scale = 0.5f / sqrtf(t);
        return Quaternion(
            q.x * scale,
            q.y * scale,
            q.z * scale,
            q.w * scale
            );
    }

    static Quaternion FromEulerRadians(const VECTOR &euler)
    {
        // Convert angles to radians and get half angles
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

        // Calculate quaternion components using ZYX order
        return Quaternion{
            cr * sp * cy + sr * cp * sy, // x
            cr * cp * sy - sr * sp * cy, // y
            sr * cp * cy - cr * sp * sy, // z
            cr * cp * cy + sr * sp * sy // w
        };
    }

    static Quaternion FromEulerDegrees(const VECTOR &euler)
    {
        return FromEulerRadians(euler * (DX_PI_F / 180.0f));
    }

    [[nodiscard]] VECTOR ToEulerRadians() const
    {
        VECTOR euler;

        // Pitch (x-axis rotation)
        const float sinp = 2.0f * (w * x + y * z);
        const float cosp = 1.0f - 2.0f * (x * x + y * y);
        euler.x = atan2f(sinp, cosp);

        // Yaw (y-axis rotation)
        const float siny = 2.0f * (w * y - z * x);
        if (abs(siny) >= 1.0f)
        {
            // Use 90 degrees if out of range
            euler.y = copysignf(DX_PI_F / 2.0f, siny);
        }
        else
        {
            euler.y = asinf(siny);
        }

        // Roll (z-axis rotation)
        const float sinr = 2.0f * (w * z + x * y);
        const float cosr = 1.0f - 2.0f * (y * y + z * z);
        euler.z = atan2f(sinr, cosr);

        return euler;
    }

    [[nodiscard]] VECTOR ToEulerDegrees() const
    {
        return ToEulerRadians() * (180.0f / DX_PI_F);
    }

    [[nodiscard]] MATRIX ToMatrix() const
    {
        MATRIX result;

        result.m[0][0] = 1 - 2 * (y * y + z * z);
        result.m[0][1] = 2 * (x * y - z * w);
        result.m[0][2] = 2 * (x * z + y * w);
        result.m[0][3] = 0.0f;

        result.m[1][0] = 2 * (x * y + z * w);
        result.m[1][1] = 1 - 2 * (x * x + z * z);
        result.m[1][2] = 2 * (y * z - x * w);
        result.m[1][3] = 0.0f;

        result.m[2][0] = 2 * (x * z - y * w);
        result.m[2][1] = 2 * (y * z + x * w);
        result.m[2][2] = 1 - 2 * (x * x + y * y);
        result.m[2][3] = 0.0f;

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }

    [[nodiscard]] Quaternion Conjugate() const
    {
        return Quaternion{-x, -y, -z, w};
    }

    [[nodiscard]] float Magnitude() const
    {
        return sqrt(x * x + y * y + z * z + w * w);
    }

    [[nodiscard]] Quaternion Inverse() const
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

    [[nodiscard]] Quaternion Normalized() const
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

    void Normalize()
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

    Quaternion operator*(const Quaternion &other) const
    {
        return {
            w * other.x + x * other.w + y * other.z - z * other.y, // x
            w * other.y - x * other.z + y * other.w + z * other.x, // y
            w * other.z + x * other.y - y * other.x + z * other.w, // z
            w * other.w - x * other.x - y * other.y - z * other.z // w
        };
    }
};

inline VECTOR operator*(const VECTOR &v, const Quaternion &q)
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