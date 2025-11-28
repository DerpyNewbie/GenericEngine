#pragma once
#include <assimp/scene.h>

#include "asset_importer.h"
#include "Asset/fbx_meta.h"

namespace engine
{
class FbxImporter : public AssetImporter
{
    using NodeToObject = std::map<const aiNode *, std::shared_ptr<ObjectMeta>>;
    // using ObjectToNode = std::map<std::shared_ptr<ObjectMeta>, const aiNode *>;
    using MeshNodes = std::set<const aiNode *>;

    struct ConversionMap
    {
        // ObjectToNode to_node;
        NodeToObject to_object;

        ConversionMap() = default;

        void Emplace(const aiNode *node, std::shared_ptr<ObjectMeta> object)
        {
            // to_node.emplace(object, node);
            to_object.emplace(node, object);
        }
    };

    std::vector<std::string> SupportedExtensions() override;
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    void OnImport(AssetDescriptor *ctx) override;

    static std::shared_ptr<ObjectMeta> CreateMapping(
        AssetDescriptor *ctx,
        const std::shared_ptr<FbxMeta> &fbx_meta,
        const aiScene *ai_scene,
        const aiNode *ai_node,
        ConversionMap &out_conversion_mapping,
        MeshNodes &out_mesh_nodes
    );

    static std::pair<AssetPtr<Mesh>, std::vector<AssetPtr<Material>>> CreateMesh(
        AssetDescriptor *ctx,
        const aiScene *ai_scene,
        const aiNode *ai_node,
        const ConversionMap &conversion_mapping
    );

    static std::shared_ptr<Material> CreateMaterial(
        AssetDescriptor *ctx,
        const aiScene *ai_scene,
        const aiMaterial *ai_material
    );
};
}