#include "pch.h"
#include "shader_importer.h"
#include "logger.h"
#include "serializer.h"
#include "Rendering/shader.h"

namespace engine
{
bool ShaderImporter::WriteShaderMeta(const std::shared_ptr<Shader> &shader, const PersistentDataStore data_store)
{
    std::stringstream string_buffer;
    {
        Serializer serializer;
        if (!serializer.Save(string_buffer, shader, false))
        {
            return false;
        }
    }

    data_store.SetString(kShaderMetaKey, string_buffer.str());
    data_store.SetInt(kShaderMetaVersionKey, kShaderMetaVersion);
    return true;
}

std::vector<std::string> ShaderImporter::SupportedExtensions()
{
    return {".hlsl"};
}

bool ShaderImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<Shader>(object) != nullptr;
}

void ShaderImporter::OnImport(AssetDescriptor *ctx)
{
    // shader_metaが無い、または壊れている場合は作り直す
    std::shared_ptr<Shader> shader;
    if (const auto shader_meta = ctx->DataStore().GetString(kShaderMetaKey);
        !shader_meta.empty())
    {
        std::stringstream ss(shader_meta);
        Serializer serializer;
        shader = serializer.Load<Shader>(ss);
        if (shader == nullptr)
            ctx->LogImportWarning("Corrupted shader meta data found! Regenerating!");
    }

    if (shader == nullptr)
        shader = Object::Instantiate<Shader>();

    std::string error_msg;
    if (!shader->Compile(ctx->AssetPath(), error_msg))
    {
        ctx->LogImportError(error_msg);
        return;
    }

    WriteShaderMeta(shader, ctx->DataStore());
    ctx->SetMainObject(shader);
}

void ShaderImporter::OnExport(AssetDescriptor *ctx)
{
    const auto shader = std::dynamic_pointer_cast<Shader>(ctx->MainObject());
    if (shader == nullptr)
    {
        ctx->LogImportError("This object cannot be exported with ShaderExporter");
        return;
    }

    WriteShaderMeta(shader, ctx->DataStore());
}
}
