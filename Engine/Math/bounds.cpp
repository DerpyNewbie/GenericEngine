#include "bounds.h"

namespace engine
{
void Bounds::Recalculate()
{
    min_point = center - extents;
    max_point = center + extents;
    size = extents * 2;
}
bool Bounds::Contains(const Vector3 &point) const
{
    return point.x >= min_point.x && point.x <= max_point.x &&
           point.y >= min_point.y && point.y <= max_point.y &&
           point.z >= min_point.z && point.z <= max_point.z;
}
}