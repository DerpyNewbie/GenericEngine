#pragma once
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class TextureCollection
{
    friend class Texture2DImporter;
    friend class Texture2D;

    inline static std::unordered_map<TextureId, AssetPtr<Texture2D>> m_textures_;


    static void SetTexture(const AssetPtr<Texture2D> &texture);
    static void RemoveTexture(const AssetPtr<Texture2D> &texture);
    static void RemoveTexture(TextureId texture_id);

public:
    static TextureId GenerateTextureId(const std::shared_ptr<Texture2D> &texture);
    static TextureId GenerateTextureId(const AssetPtr<Texture2D> &texture);
    static AssetPtr<Texture2D> GetTexture(TextureId texture_id);

};
}