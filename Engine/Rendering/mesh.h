#pragma once
#include "bone_weight.h"
#include "sub_mesh.h"
#include "CabotEngine/Graphics/VertexBuffer.h"
#include "CabotEngine/Graphics/IndexBuffer.h"

struct aiMesh;
struct aiScene;

namespace engine
{
/// <summary>
/// 頂点、index、SubMeshなどのMeshのデータを持つAssetです。
/// </summary>
class Mesh : public Object, public Inspectable
{
public:
    size_t max_bones_in_vertex = 0;

    std::vector<Vector3> vertices;
    std::vector<Color> colors; // per-vertex
    std::array<std::vector<Vector2>, 8> uvs; // per-vertex
    std::vector<uint32_t> indices; // per-face
    std::vector<Vector3> normals; // per-vertex
    std::vector<Vector4> tangents; // per-vertex
    std::vector<std::vector<BoneWeight>> bone_weights; // per-vertex
    std::vector<Matrix> bind_poses; // per-bone
    std::vector<SubMesh> sub_meshes;

    std::shared_ptr<VertexBuffer> vertex_buffer;
    std::vector<std::shared_ptr<IndexBuffer>> index_buffers;

    void OnInspectorGui() override;
    /// <summary>
    /// VertexBufferと、Mesh本体およびSubMeshごとのIndexBufferを作り直します。
    /// </summary>
    void ReconstructMeshesBuffer();

    /// <summary>
    /// AssimpのMeshから、頂点、色、UV、Index、法線、接線、Boneの情報をコピーしてMeshを作成します。
    /// </summary>
    /// <param name="mesh">変換するAssimpのMesh</param>
    static std::shared_ptr<Mesh> CreateFromAiMesh(const aiMesh *mesh);

    /// <summary>
    /// 別のMeshを、SubMeshとして末尾に結合します。
    /// </summary>
    /// <param name="other">結合するMesh</param>
    void Append(Mesh other);
    /// <summary>
    /// 指定されたチャンネルのUVのリストを取得します。
    /// </summary>
    /// <param name="index">UVのチャンネル</param>
    std::vector<Vector2> *GetUV(size_t index);
    /// <summary>
    /// 指定されたチャンネルのUVを持っているかどうかを取得します。
    /// </summary>
    /// <param name="index">UVのチャンネル</param>
    bool HasUV(size_t index) const;
    /// <summary>
    /// 接線を持っているかどうかを取得します。
    /// </summary>
    bool HasTangents() const;
    /// <summary>
    /// 法線を持っているかどうかを取得します。
    /// </summary>
    bool HasNormals() const;
    /// <summary>
    /// 頂点の色を持っているかどうかを取得します。
    /// </summary>
    bool HasColors() const;
    /// <summary>
    /// Boneのweightを持っているかどうかを取得します。
    /// </summary>
    bool HasBoneWeights() const;
    /// <summary>
    /// BindPoseを持っているかどうかを取得します。
    /// </summary>
    bool HasBindPoses() const;
    /// <summary>
    /// SubMeshを持っているかどうかを取得します。
    /// </summary>
    bool HasSubMeshes() const;

    template <class Archive>
    void serialize(Archive &archive, const uint32_t version)
    {
        archive(
            cereal::base_class<Object>(this),
            CEREAL_NVP(vertices),
            CEREAL_NVP(colors),
            CEREAL_NVP(uvs),
            CEREAL_NVP(indices),
            CEREAL_NVP(normals),
            CEREAL_NVP(tangents),
            CEREAL_NVP(bone_weights),
            CEREAL_NVP(bind_poses),
            CEREAL_NVP(sub_meshes)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::Mesh, 1)