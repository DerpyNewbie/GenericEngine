#pragma once
#include "depth_texture.h"
#include "depth_texture_buffer.h"
#include "render_texture.h"
#include "render_texture_buffer.h"
#include "texture_buffer.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
class TextureCollection
{
    friend class Texture2DImporter;
    friend class Texture2D;

    inline static std::unordered_map<AssetPtr<Texture2D>, std::shared_ptr<TextureBuffer>> m_textures_buffer_map_;
    inline static std::unordered_map<AssetPtr<RenderTexture>, std::shared_ptr<RenderTextureBuffer>> m_render_texture_buffer_map_;
    inline static std::unordered_map<AssetPtr<DepthTexture>, std::shared_ptr<DepthTextureBuffer>> m_depth_texture_buffer_map_;

public:
    static std::shared_ptr<TextureBuffer> GetTexture(const AssetPtr<Texture2D> &texture);
    static std::shared_ptr<RenderTextureBuffer> GetRenderTexture(const AssetPtr<RenderTexture> &render_texture);
    static std::shared_ptr<DepthTextureBuffer> GetDepthTexture(AssetPtr<DepthTexture> depth_texture);
};
}