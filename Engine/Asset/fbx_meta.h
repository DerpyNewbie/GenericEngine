#pragma once
#include "inspectable.h"
#include "object.h"
#include "Animation/animation_clip.h"
#include "Math/trs.h"
#include "Rendering/material.h"
#include "Rendering/mesh.h"

namespace engine
{
/// <summary>
/// FBX内の1つのNodeの情報(名前、Transform、Mesh、親子関係など)です。
/// </summary>
struct ObjectMeta
{
    // NOTE(derpy): everything is weak or shared ptr because at this point AssetPtr is unstable as it's used in import-time
    struct MeshMeta
    {
        std::weak_ptr<Mesh> instance;
        std::vector<std::weak_ptr<Material>> materials;
        std::vector<std::weak_ptr<ObjectMeta>> bones;
        std::weak_ptr<ObjectMeta> root_bone;

        template <class Archive>
        void serialize(Archive &ar, const uint32_t version)
        {
            ar(
                CEREAL_NVP(instance),
                CEREAL_NVP(materials),
                CEREAL_NVP(bones),
                CEREAL_NVP(root_bone)
            );
        }
    };

    std::string name;
    TRS local_transform;
    MeshMeta mesh;
    std::weak_ptr<AnimationClip> animation;

    std::weak_ptr<ObjectMeta> parent;
    std::vector<std::shared_ptr<ObjectMeta>> children;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(name),
            CEREAL_NVP(local_transform),
            CEREAL_NVP(mesh),
            CEREAL_NVP(animation),
            CEREAL_NVP(parent),
            CEREAL_NVP(children)
        );
    }
};

/// <summary>
/// ImportしたFBXの階層の情報を持ち、GameObjectとして生成できるAssetです。
/// </summary>
class FbxMeta : public Object, public Inspectable
{
public:
    std::shared_ptr<ObjectMeta> root_object_meta;
    std::map<std::shared_ptr<ObjectMeta>, std::pair<AssetPtr<Mesh>, std::vector<AssetPtr<Material>>>> mesh_objects;
    
    void OnInspectorGui() override;

    /// <summary>
    /// FBXの階層構造からGameObjectを生成し、Meshを持つものにMeshRendererまたはSkinnedMeshRendererを設定します。
    /// </summary>
    /// <returns>生成されたRootのGameObject</returns>
    std::shared_ptr<GameObject> Instantiate() const;

    template <class Archive>
    void serialize(Archive &ar, const uint32_t version)
    {
        ar(
            CEREAL_NVP(root_object_meta)
        );
    }
};
}

CEREAL_CLASS_VERSION(engine::ObjectMeta, 1)

CEREAL_CLASS_VERSION(engine::ObjectMeta::MeshMeta, 1)

CEREAL_CLASS_VERSION(engine::FbxMeta, 1)