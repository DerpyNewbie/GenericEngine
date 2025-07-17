#pragma once

namespace engine
{
using namespace DirectX::SimpleMath;

struct Bounds
{
    Vector3 center;
    Vector3 extents;
    Vector3 min_point;
    Vector3 max_point;
    Vector3 size;

    constexpr Bounds() = default;

    Bounds(const Vector3 &center, const Vector3 &extents) : center(center), extents(extents),
                                                            min_point(center - extents), max_point(center + extents),
                                                            size(extents * 2)
    {
    }

    static Bounds FromMinMax(const Vector3 &min_point, const Vector3 &max_point)
    {
        Bounds result;
        result.center = (max_point + min_point) / 2;
        result.extents = (max_point - min_point) / 2;
        result.min_point = min_point;
        result.max_point = max_point;
        result.size = max_point - min_point;

        return result;
    }

    void Recalculate();

    [[nodiscard]] bool Contains(const Vector3 &point) const;
};
}