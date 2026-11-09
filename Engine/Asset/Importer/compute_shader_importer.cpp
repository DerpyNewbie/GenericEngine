#include "pch.h"
#include "compute_shader_importer.h"
#include "serializer.h"
#include "Rendering/compute_shader.h"

namespace engine
{
bool ComputeShaderImporter::WriteMeta(const std::shared_ptr<ComputeShader> &shader, const PersistentDataStore data_store)
{
    std::stringstream string_buffer;
    {
        Serializer serializer;
        if (!serializer.Save(string_buffer, shader, false))
        {
            return false;
        }
    }

    data_store.SetString(kComputeShaderMetaKey, string_buffer.str());
    return true;
}

std::vector<std::string> ComputeShaderImporter::SupportedExtensions()
{
    return {".compute"};
}

bool ComputeShaderImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<ComputeShader>(object) != nullptr;
}

void ComputeShaderImporter::OnImport(AssetDescriptor *ctx)
{
    // metaが無い、または壊れている場合は作り直す
    std::shared_ptr<ComputeShader> shader;
    if (const auto meta = ctx->DataStore().GetString(kComputeShaderMetaKey);
        !meta.empty())
    {
        std::stringstream ss(meta);
        Serializer serializer;
        shader = serializer.Load<ComputeShader>(ss);
        if (shader == nullptr)
            ctx->LogImportWarning("Corrupted compute shader meta data found! Regenerating!");
    }

    if (shader == nullptr)
        shader = Object::Instantiate<ComputeShader>();

    std::string error_msg;
    if (!shader->Compile(ctx->AssetPath(), error_msg))
    {
        ctx->LogImportError(error_msg);
        return;
    }

    WriteMeta(shader, ctx->DataStore());
    ctx->SetMainObject(shader);
}

void ComputeShaderImporter::OnExport(AssetDescriptor *ctx)
{
    const auto shader = std::dynamic_pointer_cast<ComputeShader>(ctx->MainObject());
    if (shader == nullptr)
    {
        ctx->LogImportError("This object cannot be exported with ComputeShaderImporter");
        return;
    }

    WriteMeta(shader, ctx->DataStore());
}
}