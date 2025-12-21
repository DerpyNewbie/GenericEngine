#pragma once
#include <assimp/scene.h>

#include "asset_importer.h"
#include "Asset/fbx_meta.h"

namespace engine
{
class FbxImporter : public AssetImporter
{
    using NodeToObject = std::map<const aiNode *, std::shared_ptr<ObjectMeta>>;
    using IdxToMaterial = std::map<unsigned int, std::shared_ptr<Material>>;
    using IdxToTexture = std::map<unsigned int, std::shared_ptr<Texture2D>>;
    using MeshNodes = std::set<const aiNode *>;

    struct ConversionMap
    {
        NodeToObject to_object;
        IdxToMaterial to_material;
        IdxToTexture to_texture;

        ConversionMap() = default;

        void EmplaceObject(const aiNode *node, std::shared_ptr<ObjectMeta> object)
        {
            to_object.emplace(node, object);
        }

        void EmplaceMaterial(unsigned int idx, std::shared_ptr<Material> material)
        {
            to_material.emplace(idx, material);
        }

        void EmplaceTexture(unsigned int idx, std::shared_ptr<Texture2D> texture)
        {
            to_texture.emplace(idx, texture);
        }
    };

    std::vector<std::string> SupportedExtensions() override;
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    void OnImport(AssetDescriptor *ctx) override;

    static std::shared_ptr<ObjectMeta> CreateNodeMappings(
        AssetDescriptor *ctx,
        const std::shared_ptr<FbxMeta> &fbx_meta,
        const aiScene *ai_scene,
        const aiNode *ai_node,
        ConversionMap &out_conversion_mapping,
        MeshNodes &out_mesh_nodes
    );

    static void CreateTextureMappings(
        AssetDescriptor *ctx,
        const aiScene *ai_scene,
        ConversionMap &out_conversion_mapping
    );

    static void CreateMaterialMappings(
        AssetDescriptor *ctx,
        const aiScene *ai_scene,
        ConversionMap &out_conversion_mapping
    );

    static std::pair<AssetPtr<Mesh>, std::vector<AssetPtr<Material>>> CreateMesh(
        AssetDescriptor *ctx,
        const aiScene *ai_scene,
        const aiNode *ai_node,
        const ConversionMap &convert
    );

    static std::shared_ptr<AnimationClip> CreateAnimationClip(
        AssetDescriptor *ctx,
        const aiAnimation *ai_animation
    );
};
}