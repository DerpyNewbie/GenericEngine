#include "pch.h"
#include "effekseer_effect_importer.h"

#include "Asset/effekseer_effect.h"

std::vector<std::string> engine::EffekseerEffectImporter::SupportedExtensions()
{
    return {".efkefc"};
}

bool engine::EffekseerEffectImporter::IsCompatibleWith(std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<EffekseerEffect>(object) != nullptr;
}

void engine::EffekseerEffectImporter::OnImport(AssetDescriptor *ctx)
{
    const auto effect = Object::Instantiate<EffekseerEffect>();

    effect->path = ctx->AssetPath().string();
    ctx->SetMainObject(effect);
}