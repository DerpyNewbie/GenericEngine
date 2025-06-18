#pragma once
#include <directxtk12/SimpleMath.h>
#include "bounds.h"
#include "color.h"

using namespace DirectX::SimpleMath;

namespace cereal
{
template <class Archive>
void serialize(Archive &ar, engine::Color &v)
{
    ar(v.r, v.g, v.b, v.a);
}
template <class Archive>
void serialize(Archive &ar, engine::Bounds &v)
{
    ar(v.center, v.extents, v.size, v.min_point, v.max_point);
}

template <class Archive>
void serialize(Archive &ar, Vector2 &v)
{
    ar(CEREAL_NVP(v.x), CEREAL_NVP(v.y));
}
template <class Archive>
void serialize(Archive &ar, Vector3 &v)
{
    ar(CEREAL_NVP(v.x), CEREAL_NVP(v.y), CEREAL_NVP(v.z));
}
template <class Archive>
void serialize(Archive &ar, Vector4 &v)
{
    ar(CEREAL_NVP(v.x), CEREAL_NVP(v.y), CEREAL_NVP(v.z), CEREAL_NVP(v.w));
}
template <class Archive>
void serialize(Archive &ar, Matrix &v)
{
    ar(CEREAL_NVP(v.m[0][0]), CEREAL_NVP(v.m[0][1]), CEREAL_NVP(v.m[0][2]), CEREAL_NVP(v.m[0][3]),
       CEREAL_NVP(v.m[1][0]), CEREAL_NVP(v.m[1][1]), CEREAL_NVP(v.m[1][2]), CEREAL_NVP(v.m[1][3]),
       CEREAL_NVP(v.m[2][0]), CEREAL_NVP(v.m[2][1]), CEREAL_NVP(v.m[2][2]), CEREAL_NVP(v.m[2][3]),
       CEREAL_NVP(v.m[3][0]), CEREAL_NVP(v.m[3][1]), CEREAL_NVP(v.m[3][2]), CEREAL_NVP(v.m[3][3]));
}
template <class Archive>
void serialize(Archive &ar, Quaternion &v)
{
    ar(CEREAL_NVP(v.x), CEREAL_NVP(v.y), CEREAL_NVP(v.z), CEREAL_NVP(v.w));
}
template <class Archive>
void serialize(Archive &ar, Plane &v)
{
    ar(CEREAL_NVP(v.x), CEREAL_NVP(v.y), CEREAL_NVP(v.z), CEREAL_NVP(v.w));
}
template <class Archive>
void serialize(Archive &ar, struct Rectangle &v)
{
    ar(CEREAL_NVP(v.x), CEREAL_NVP(v.y), CEREAL_NVP(v.width), CEREAL_NVP(v.height));
}

}