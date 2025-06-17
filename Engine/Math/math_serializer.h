#pragma once
#include "bounds.h"
#include "color.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"
#include "matrix4x4.h"

namespace cereal
{
template <class Archive>
void serialize(Archive &ar, engine::Bounds &v)
{
    ar(v.center, v.extents, v.size, v.min_point, v.max_point);
}
template <class Archive>
void serialize(Archive &ar, engine::Color &v)
{
    ar(v.r, v.g, v.b, v.a);
}
template <class Archive>
void serialize(Archive &archive, engine::Matrix4x4 &v)
{
    archive(v.m[0][0], v.m[0][1], v.m[0][2], v.m[0][3],
            v.m[1][0], v.m[1][1], v.m[1][2], v.m[1][3],
            v.m[2][0], v.m[2][1], v.m[2][2], v.m[2][3],
            v.m[3][0], v.m[3][1], v.m[3][2], v.m[3][3]);
}
template <class Archive>
void serialize(Archive &ar, engine::Quaternion v)
{
    ar(v.x, v.y, v.z, v.w);
}
template <class Archive>
void serialize(Archive &ar, engine::Vector2 v)
{
    ar(v.x, v.y);
}
template <class Archive>
void serialize(Archive &ar, engine::Vector3 v)
{
    ar(v.x, v.y, v.z);
}
template <class Archive>
void serialize(Archive &ar, engine::Vector4 v)
{
    ar(v.x, v.y, v.z, v.w);
}

}