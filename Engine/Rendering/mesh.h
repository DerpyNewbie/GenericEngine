#pragma once
#include "Math/color.h"
#include "Math/matrix4x4.h"
#include "Math/vector3.h"
#include "Math/vector4.h"
#include "bone_weight.h"
#include "sub_mesh.h"

#include <vector>

namespace engine
{
struct Mesh
{
    std::vector<Vector3> vertices;
    std::vector<Color> colors;
    std::vector<Vector2> uv;
    std::vector<Vector2> uv2;
    std::vector<Vector2> uv3;
    std::vector<Vector2> uv4;
    std::vector<Vector2> uv5;
    std::vector<Vector2> uv6;
    std::vector<Vector2> uv7;
    std::vector<int> indices;
    std::vector<Vector3> normals;
    std::vector<Vector4> tangents;
    std::vector<BoneWeight> bone_weights;
    std::vector<Matrix4x4> bind_poses;
    std::vector<SubMesh> sub_meshes;

    static Mesh CreateFromMV1(int model_handle);
};
}