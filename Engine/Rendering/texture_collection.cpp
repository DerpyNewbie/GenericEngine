#include "pch.h"
#include "texture_collection.h"

void engine::TextureCollection::SetTexture(const AssetPtr<Texture2D> &texture)
{
    if (texture == nullptr)
        return;

    const auto texture_id = GenerateTextureId(texture);
    m_textures_[texture_id] = texture;
}

void engine::TextureCollection::RemoveTexture(const AssetPtr<Texture2D> &texture)
{
    RemoveTexture(GenerateTextureId(texture));
}

void engine::TextureCollection::RemoveTexture(const TextureId texture_id)
{
    m_textures_.erase(texture_id);
}

engine::TextureId engine::TextureCollection::GenerateTextureId(const std::shared_ptr<Texture2D> &texture)
{
    return reinterpret_cast<TextureId>(texture.get());
}

engine::TextureId engine::TextureCollection::GenerateTextureId(const AssetPtr<Texture2D> &texture)
{
    return GenerateTextureId(texture.CastedLock());
}

engine::AssetPtr<engine::Texture2D> engine::TextureCollection::GetTexture(const TextureId texture_id)
{
    const auto it = m_textures_.find(texture_id);
    if (it != m_textures_.end())
    {
        return it->second;
    }

    return {};
}