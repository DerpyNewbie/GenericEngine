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
    // using ObjectToNode = std::map<std::shared_ptr<ObjectMeta>, const aiNode *>;
    using MeshNodes = std::set<const aiNode *>;

    struct ConversionMap
    {
        // ObjectToNode to_node;
        NodeToObject to_object;
        IdxToMaterial to_material;

        ConversionMap() = default;

        void EmplaceObject(const aiNode *node, std::shared_ptr<ObjectMeta> object)
        {
            to_object.emplace(node, object);
        }

        void EmplaceMaterial(unsigned int idx, std::shared_ptr<Material> material)
        {
            to_material.emplace(idx, material);
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


    static void CreateMaterialMappings(
        AssetDescriptor *ctx,
        const aiScene *ai_scene,
        ConversionMap &conversion_mapping
    );

    static std::pair<AssetPtr<Mesh>, std::vector<AssetPtr<Material>>> CreateMesh(
        AssetDescriptor *ctx,
        const aiScene *ai_scene,
        const aiNode *ai_node,
        const ConversionMap &conversion_mapping
    );
};
}