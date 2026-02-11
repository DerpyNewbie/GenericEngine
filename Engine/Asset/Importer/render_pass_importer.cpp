#include "pch.h"
#include "render_pass_importer.h"

#include "serializer.h"
#include "Rendering/render_pass.h"

std::vector<std::string> engine::RenderPassImporter::SupportedExtensions()
{
    return {".renderpass"};
}
bool engine::RenderPassImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return AssetImporter::IsCompatibleWith(object);
}

void engine::RenderPassImporter::OnImport(AssetDescriptor *ctx)
{
    std::ifstream file(ctx->AssetPath());
    Serializer serializer;

    ctx->SetMainObject(serializer.Load<RenderPass>(file));
}

void engine::RenderPassImporter::OnExport(AssetDescriptor *ctx)
{
    const auto material = std::dynamic_pointer_cast<RenderPass>(ctx->MainObject());
    if (material == nullptr)
    {
        assert(false && "This object cannot be exported with RenderPassExporter");
    }

    std::ofstream file(ctx->AssetPath());
    Serializer serializer;
    assert(serializer.Save(file, material) && "Something is wrong with this renderpass instance");
}