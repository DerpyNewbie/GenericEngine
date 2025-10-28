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

bool MaterialImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<Material>(object) != nullptr;
}

void MaterialImporter::OnImport(AssetDescriptor *ctx)
{
    std::ifstream file(ctx->AssetPath());
    Serializer serializer;

    ctx->SetMainObject(serializer.Load<Material>(file));
}

void MaterialImporter::OnExport(AssetDescriptor *ctx)
{
    const auto material = std::dynamic_pointer_cast<Material>(ctx->MainObject());
    if (material == nullptr)
    {
        assert(false && "This object cannot be exported with MaterialExporter");
    }

    std::ofstream file(ctx->AssetPath());
    Serializer serializer;
    serializer.Save(file, material);
}
}