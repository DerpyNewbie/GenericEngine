#pragma once
#include "Math/color.h"
#include "Math/matrix4x4.h"
#include "Math/vector3.h"
#include "Math/vector4.h"
#include "bone_weight.h"
#include "object.h"
#include "sub_mesh.h"

#include <vector>
#include "rendering_serializer.h"

namespace engine
{
struct Mesh : Object
{
    std::vector<Vector3> vertices;
    std::vector<Color> colors;
    std::vector<Vector2> uv;
    std::vector<Vector2> uv2;
    std::vector<short> indices;
    std::vector<Vector3> normals;
    std::vector<Vector4> tangents;
    std::vector<unsigned char> bones_per_vertex;
    std::vector<BoneWeight> bone_weights;
    std::vector<Matrix4x4> bind_poses;
    std::vector<SubMesh> sub_meshes;

    static std::vector<std::shared_ptr<Mesh>> CreateFromMV1(int model_handle, int frame_index);
    static std::shared_ptr<Mesh> CreateFromMV1ReferenceMesh(const MV1_REF_POLYGONLIST &mv1_ref_polygon_list);

    void Append(Mesh other);

    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(
            cereal::base_class<Object>(this),
            CEREAL_NVP(vertices),
            CEREAL_NVP(colors),
            CEREAL_NVP(uv), CEREAL_NVP(uv2),
            CEREAL_NVP(indices),
            CEREAL_NVP(normals), CEREAL_NVP(tangents),
            CEREAL_NVP(bone_weights), CEREAL_NVP(bind_poses),
            CEREAL_NVP(sub_meshes));
    }
};
}