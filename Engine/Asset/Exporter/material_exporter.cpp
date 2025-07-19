#include "material_exporter.h"
#include "serializer.h"
#include "Asset/asset_descriptor.h"

std::vector<std::string> MaterialExporter::SupportedExtensions()
{
    return {".material"};
}

void MaterialExporter::Export(engine::AssetDescriptor *descriptor)
{
    std::string file_path = descriptor->managed_object->Name() + ".material";
    std::ofstream ofs(file_path);
    engine::Serializer serializer;

    serializer.Save(ofs, descriptor->managed_object);
}