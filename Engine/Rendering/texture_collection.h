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

    inline static std::unordered_map<std::shared_ptr<Texture2D>, std::shared_ptr<TextureBuffer>> m_textures_buffer_map_;
    inline static std::unordered_map<std::shared_ptr<RenderTexture>, std::shared_ptr<RenderTextureBuffer>> m_render_texture_buffer_map_;
    inline static std::unordered_map<std::shared_ptr<DepthTexture>, std::shared_ptr<DepthTextureBuffer>> m_depth_texture_buffer_map_;

public:
    static std::shared_ptr<TextureBuffer> GetTexture(const std::shared_ptr<Texture2D> &texture);
    static std::shared_ptr<RenderTextureBuffer> GetRenderTexture(const std::shared_ptr<RenderTexture> &render_texture);
    static std::shared_ptr<DepthTextureBuffer> GetDepthTexture(std::shared_ptr<DepthTexture> depth_texture);

    static bool DeleteTexture(const std::shared_ptr<Texture2D> &texture);
    static bool DeleteRenderTexture(const std::shared_ptr<RenderTexture> &render_texture);
    static bool DeleteDepthTexture(const std::shared_ptr<DepthTexture> &depth_texture);
};
}