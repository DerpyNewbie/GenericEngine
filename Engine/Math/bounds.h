#pragma once
#include "vector3.h"

namespace engine
{
struct Bounds
{
    Vector3 center;
    Vector3 extents;
    Vector3 min_point;
    Vector3 max_point;
    Vector3 size;

    constexpr Bounds() = default;
    constexpr Bounds(const Vector3 &center, const Vector3 &extents) : center(center), extents(extents),
        min_point(center - extents), max_point(center + extents), size(extents * 2)
    {}

    void Recalculate();

    [[nodiscard]] bool Contains(const Vector3 &point) const;
};
}