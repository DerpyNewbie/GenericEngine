#include "pch.h"
#include "raytracing_shader_importer.h"

#include "Rendering/raytracing/raytracing_pipeline_state.h"
#include "Rendering/raytracing/raytracing_shader.h"

std::vector<std::string> engine::RaytracingShaderImporter::SupportedExtensions()
{
    return {".raytrace"};
}

bool engine::RaytracingShaderImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<RaytracingShader>(object) != nullptr;
}

void engine::RaytracingShaderImporter::OnImport(AssetDescriptor *ctx)
{
    const auto raytracing_shader = std::make_shared<RaytracingShader>(ctx->AssetPath());
    RaytracingPipelineState::Instance()->CreateDxrPipelineState(raytracing_shader);
    ctx->SetMainObject(raytracing_shader);
}