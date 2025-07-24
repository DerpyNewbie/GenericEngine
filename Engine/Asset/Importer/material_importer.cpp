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

std::shared_ptr<Object> MaterialImporter::Import(std::istream &input_stream, AssetDescriptor *asset)
{
    Serializer serializer;
    return serializer.Load<Material>(input_stream);
}
}