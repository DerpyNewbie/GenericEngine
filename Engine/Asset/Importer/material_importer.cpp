#include "pch.h"
#include "material_importer.h"

std::vector<std::string> MaterialImporter::SupportedExtensions()
{
    return {".material"};
}

std::shared_ptr<engine::Object> MaterialImporter::Import(engine::AssetDescriptor *asset)
{
    //TODO:Add Code
    return nullptr;
}