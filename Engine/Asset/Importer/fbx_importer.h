#pragma once
#include <assimp/scene.h>

#include "asset_importer.h"
#include "Asset/fbx_meta.h"

namespace engine
{
/// <summary>
/// FBXファイルをImportし、Mesh、Material、Texture、AnimationClipなどを作成するImporterです。
/// </summary>
class FbxImporter : public AssetImporter
{
    using NodeToObject = std::map<const aiNode *, std::shared_ptr<ObjectMeta>>;
    using IdxToMaterial = std::map<unsigned int, std::shared_ptr<Material>>;
    using IdxToTexture = std::map<unsigned int, std::shared_ptr<Texture2D>>;
    using MeshNodes = std::set<const aiNode *>;

    /// <summary>
    /// assimpのNodeやindexから、作成したObject、Material、Textureへの対応表です。
    /// </summary>
    struct ConversionMap
    {
        NodeToObject to_object;
        IdxToMaterial to_material;
        IdxToTexture to_texture;

        ConversionMap() = default;

        /// <summary>
        /// AssimpのNodeとObjectMetaの対応を記録します。
        /// </summary>
        void EmplaceObject(const aiNode *node, std::shared_ptr<ObjectMeta> object)
        {
            to_object.emplace(node, object);
        }

        /// <summary>
        /// AssimpのMaterialのindexとMaterialの対応を記録します。
        /// </summary>
        void EmplaceMaterial(unsigned int idx, std::shared_ptr<Material> material)
        {
            to_material.emplace(idx, material);
        }

        /// <summary>
        /// Assimpの埋め込みTextureのindexとTexture2Dの対応を記録します。
        /// </summary>
        void EmplaceTexture(unsigned int idx, std::shared_ptr<Texture2D> texture)
        {
            to_texture.emplace(idx, texture);
        }
    };

    /// <summary>
    /// 対応する拡張子(".fbx")を返します。
    /// </summary>
    std::vector<std::string> SupportedExtensions() override;
    /// <summary>
    /// 常に false を返します。FBXの書き出しには対応していません。
    /// </summary>
    bool IsCompatibleWith(std::shared_ptr<Object> object) override;
    /// <summary>
    /// AssimpでFBXを読み込み、FbxMeta、Texture、Material、Mesh、AnimationClipを生成します。
    /// </summary>
    void OnImport(AssetDescriptor *ctx) override;

    /// <summary>
    /// Nodeとその子からObjectMetaの階層を再帰的に生成し、NodeとObjectMetaの対応を記録します。
    /// </summary>
    /// <param name="ai_node">変換するNode</param>
    /// <param name="out_conversion_mapping">対応の記録先</param>
    /// <param name="out_mesh_nodes">Meshを持つNodeの記録先</param>
    /// <returns>生成されたObjectMeta</returns>
    static std::shared_ptr<ObjectMeta> CreateNodeMappings(
        AssetDescriptor *ctx,
        const std::shared_ptr<FbxMeta> &fbx_meta,
        const aiScene *ai_scene,
        const aiNode *ai_node,
        ConversionMap &out_conversion_mapping,
        MeshNodes &out_mesh_nodes
    );

    /// <summary>
    /// FBXに埋め込まれたTextureからTexture2Dを生成し、Assetに追加します。
    /// </summary>
    /// <param name="out_conversion_mapping">対応の記録先</param>
    static void CreateTextureMappings(
        AssetDescriptor *ctx,
        const aiScene *ai_scene,
        ConversionMap &out_conversion_mapping
    );

    /// <summary>
    /// FBXのMaterialからMaterialを生成し、Albedoのテクスチャを設定してAssetに追加します。
    /// </summary>
    /// <param name="out_conversion_mapping">対応の記録先</param>
    static void CreateMaterialMappings(
        AssetDescriptor *ctx,
        const aiScene *ai_scene,
        ConversionMap &out_conversion_mapping
    );

    /// <summary>
    /// Nodeが持つMeshを1つのMeshにまとめて生成し、MaterialとBoneの参照を設定します。
    /// </summary>
    /// <param name="ai_node">Meshを持つNode</param>
    /// <param name="convert">生成済みのObjectの対応</param>
    /// <returns>生成されたMeshと、使用するMaterialのリスト</returns>
    static std::pair<AssetPtr<Mesh>, std::vector<AssetPtr<Material>>> CreateMesh(
        AssetDescriptor *ctx,
        const aiScene *ai_scene,
        const aiNode *ai_node,
        const ConversionMap &convert
    );

    /// <summary>
    /// FBXのAnimationからAnimationClipを生成し、Assetに追加します。
    /// </summary>
    /// <param name="ai_animation">変換するAnimation</param>
    static std::shared_ptr<AnimationClip> CreateAnimationClip(
        AssetDescriptor *ctx,
        const aiAnimation *ai_animation
    );
};
}