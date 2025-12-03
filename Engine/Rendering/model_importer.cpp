#include "pch.h"
#include "Asset/asset_database.h"
#include "Asset/fbx_meta.h"
#include "Asset/Importer/fbx_importer.h"
#include "model_importer.h"
#include "game_object.h"

namespace engine
{
std::shared_ptr<GameObject> ModelImporter::LoadModelFromFBX(const char *file_path)
{
    const auto asset_desc = AssetDatabase::GetAssetDescriptor(AssetDatabase::GetAsset(file_path).Guid());
    const auto fbx_meta = std::dynamic_pointer_cast<FbxMeta>(asset_desc->MainObject());
    if (fbx_meta == nullptr)
    {
        Logger::Error<ModelImporter>("Failed to load model from FBX! No FbxMeta found!");
        return nullptr;
    }

    return fbx_meta->Instantiate();
}
}