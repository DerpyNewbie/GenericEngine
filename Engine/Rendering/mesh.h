#pragma once
#include <vector>
#include "Math/bounds.h"
#include "bone_weight.h"
#include "object.h"
#include "sub_mesh.h"
#include "rendering_serializer.h"

struct aiMesh;
struct aiScene;

namespace engine
{
class Mesh : public Object
{
public:
    unsigned char max_bones_per_vertex;
    
    Bounds bounds;
    std::vector<Vector3> vertices;
    std::vector<Color> colors;
    std::array<std::vector<Vector2>, 8> uvs;
    std::vector<uint32_t> indices;
    std::vector<Vector3> normals;
    std::vector<Vector4> tangents;
    std::vector<std::vector<BoneWeight>> bone_weights;
    std::vector<Matrix> bind_poses;
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
        // FIXME: something is wrong in here
        archive(
            cereal::base_class<Object>(this),
            CEREAL_NVP(vertices),
            CEREAL_NVP(colors),
            CEREAL_NVP(bounds),
            CEREAL_NVP(uvs), 
            CEREAL_NVP(indices),
            CEREAL_NVP(normals), CEREAL_NVP(tangents),
            CEREAL_NVP(bone_weights),
            CEREAL_NVP(bind_poses),
            CEREAL_NVP(sub_meshes));
    }
};
}