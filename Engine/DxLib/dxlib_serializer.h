#pragma once
#include <DxLib.h>

namespace cereal
{
template <class Archive>
void serialize(Archive &archive, VECTOR &value)
{
    archive(value.x, value.y, value.z);
}

template <class Archive>
void serialize(Archive &archive, MATRIX &matrix)
{
    archive(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2], matrix.m[0][3],
            matrix.m[1][0], matrix.m[1][1], matrix.m[1][2], matrix.m[1][3],
            matrix.m[2][0], matrix.m[2][1], matrix.m[2][2], matrix.m[2][3],
            matrix.m[3][0], matrix.m[3][1], matrix.m[3][2], matrix.m[3][3]);
}
}