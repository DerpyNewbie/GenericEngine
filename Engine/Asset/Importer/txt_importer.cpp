#include "pch.h"

#include "txt_importer.h"

#include "Asset/text_asset.h"

namespace engine
{
std::vector<std::string> TxtImporter::SupportedExtensions()
{
    return {".txt"};
}

bool TxtImporter::IsCompatibleWith(const std::shared_ptr<Object> object)
{
    return std::dynamic_pointer_cast<TextAsset>(object) != nullptr;
}

void TxtImporter::OnImport(AssetDescriptor *ctx)
{
    const auto text = Object::Instantiate<TextAsset>();
    const std::ifstream input_stream(ctx->AssetPath());
    std::stringstream ss;
    ss << input_stream.rdbuf();
    text->content = ss.str();

    for (const auto &key : ctx->DataStore().GetKeys())
    {
        text->key_value_pairs[key] = ctx->DataStore().GetString(key);
    }

    ctx->SetMainObject(text);

    const auto secondary = Object::Instantiate<TextAsset>();
    ctx->AddObject(secondary);
}

void TxtImporter::OnExport(AssetDescriptor *ctx)
{
    const auto text = std::dynamic_pointer_cast<TextAsset>(ctx->MainObject());
    if (text == nullptr)
    {
        Logger::Warn<TxtImporter>("Asset '%s' has not been imported or not a TextAsset",
                                  ctx->AssetPath().string().c_str());
        return;
    }

    std::ofstream file(ctx->AssetPath());
    file << text->content;
    file.close();

    auto &data_store = ctx->DataStore();
    data_store.ClearKeys();
    for (auto [key, value] : text->key_value_pairs)
    {
        ctx->DataStore().SetString(key, value);
    }
}
}