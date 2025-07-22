#include "pch.h"
#include "material_importer.h"

#include "serializer.h"
#include "Rendering/material.h"

namespace engine
{
std::vector<std::string> MaterialImporter::SupportedExtensions()
{
    return {".material"};
}

std::shared_ptr<Object> MaterialImporter::Import(AssetDescriptor *asset)
{
    Serializer serializer;
    std::ifstream ifs(asset->path_hint);
    return serializer.Load<Material>(ifs);
}
}