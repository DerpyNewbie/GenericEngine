#include "material_exporter.h"
#include "serializer.h"
#include "Asset/asset_descriptor.h"
#include "Rendering/material.h"

namespace engine
{
std::vector<std::string> MaterialExporter::SupportedExtensions()
{
    return {".material"};
}

void MaterialExporter::Export(std::ostream &output_stream, AssetDescriptor *descriptor)
{
    const auto material = std::dynamic_pointer_cast<Material>(descriptor->managed_object);
    if (material == nullptr)
    {
        assert(false && "This object cannot be exported with MaterialExporter");
    }

    Serializer serializer;
    serializer.Save(output_stream, material);
}

bool MaterialExporter::CanExport(const std::shared_ptr<Object> &object)
{
    return std::dynamic_pointer_cast<Material>(object) != nullptr;
}
}