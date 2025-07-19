#pragma once
#include "Math/bounds.h"
#include "bone_weight.h"
#include "sub_mesh.h"

struct aiMesh;
struct aiScene;

namespace engine
{
class Mesh : public Object
{
public:
    unsigned int max_bones_in_vertex = 0;

    std::vector<Vector3> vertices;
    std::vector<Color> colors; // per-vertex
    std::array<std::vector<Vector2>, 8> uvs; // per-vertex
    std::vector<uint32_t> indices; // per-face
    std::vector<Vector3> normals; // per-vertex
    std::vector<Vector4> tangents; // per-vertex
    std::vector<std::vector<BoneWeight>> bone_weights; // per-vertex
    std::vector<Matrix> bind_poses; // per-bone
    std::vector<SubMesh> sub_meshes;

    static std::shared_ptr<Mesh> CreateFromAiMesh(const aiMesh *mesh);

    // TODO: remove DxLib dependency
    static std::vector<std::shared_ptr<Mesh>> CreateFromMV1(int model_handle, int frame_index);

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
            CEREAL_NVP(uvs),
            CEREAL_NVP(indices),
            CEREAL_NVP(normals), CEREAL_NVP(tangents),
            CEREAL_NVP(bone_weights),
            CEREAL_NVP(bind_poses),
            CEREAL_NVP(sub_meshes));
    }
};
}