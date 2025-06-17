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
#include "Math/bounds.h"

struct aiMesh;
struct aiScene;

namespace engine
{
class Mesh : public Object
{
public:
    std::vector<Vector3> vertices;
    std::vector<Color> colors;
    Bounds bounds;
    std::vector<Vector2> uv;
    std::vector<Vector2> uv2;
    std::vector<Vector2> uv3;
    std::vector<Vector2> uv4;
    std::vector<Vector2> uv5;
    std::vector<Vector2> uv6;
    std::vector<Vector2> uv7;
    std::vector<Vector2> uv8;
    std::vector<short> indices;
    std::vector<Vector3> normals;
    std::vector<Vector4> tangents;
    std::vector<unsigned char> bones_per_vertex;
    std::vector<BoneWeight> bone_weights;
    std::vector<Matrix4x4> bind_poses;
    std::vector<SubMesh> sub_meshes;

    static std::shared_ptr<Mesh> CreateFromAiMesh(const aiScene *scene, const aiMesh *mesh);
    static std::vector<std::shared_ptr<Mesh>> CreateFromMV1(int model_handle, int frame_index);
    static std::shared_ptr<Mesh> CreateFromMV1ReferenceMesh(const MV1_REF_POLYGONLIST &mv1_ref_polygon_list);

    void Append(Mesh other);
    std::vector<Vector2> *GetUV(int index);
    bool HasUV(int index) const;
    bool HasTangents() const;
    bool HasNormals() const;
    bool HasColors() const;
    bool HasBoneWeights() const;
    bool HasBindPoses() const;
    bool HasSubMeshes() const;

    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(
            cereal::base_class<Object>(this),
            CEREAL_NVP(vertices),
            CEREAL_NVP(colors),
            CEREAL_NVP(bounds),
            CEREAL_NVP(uv), CEREAL_NVP(uv2),
            CEREAL_NVP(uv3), CEREAL_NVP(uv4),
            CEREAL_NVP(uv5), CEREAL_NVP(uv6),
            CEREAL_NVP(uv7), CEREAL_NVP(uv8),
            CEREAL_NVP(indices),
            CEREAL_NVP(normals), CEREAL_NVP(tangents),
            CEREAL_NVP(bones_per_vertex), CEREAL_NVP(bone_weights),
            CEREAL_NVP(bind_poses),
            CEREAL_NVP(sub_meshes));
    }
};
}