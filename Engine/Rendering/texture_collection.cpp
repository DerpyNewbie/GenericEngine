#include "pch.h"
#include "texture_collection.h"

std::shared_ptr<engine::TextureBuffer> engine::TextureCollection::GetTexture(const AssetPtr<Texture2D> &texture)
{
    const auto it = m_textures_buffer_map_.find(texture);
    if (it != m_textures_buffer_map_.end())
        return it->second;

    auto texture_buffer = std::make_shared<TextureBuffer>(texture.CastedLock());
    m_textures_buffer_map_.try_emplace(texture, texture_buffer);

    return texture_buffer;
}

std::shared_ptr<engine::RenderTextureBuffer> engine::TextureCollection::GetRenderTexture(const AssetPtr<RenderTexture> &render_texture)
{
    const auto it = m_render_texture_buffer_map_.find(render_texture);
    if (it != m_render_texture_buffer_map_.end())
        return it->second;

    auto render_texture_buffer = std::make_shared<RenderTextureBuffer>(render_texture);
    m_textures_buffer_map_.try_emplace(render_texture, render_texture_buffer);
    m_render_texture_buffer_map_.try_emplace(render_texture, render_texture_buffer);

    return render_texture_buffer;
}

std::shared_ptr<engine::DepthTextureBuffer> engine::TextureCollection::GetDepthTexture(AssetPtr<DepthTexture> depth_texture)
{
    const auto it = m_depth_texture_buffer_map_.find(depth_texture);
    if (it != m_depth_texture_buffer_map_.end())
        return it->second;

    auto depth_texture_buffer = std::make_shared<DepthTextureBuffer>(depth_texture);
    m_textures_buffer_map_.try_emplace(depth_texture, depth_texture_buffer);
    m_render_texture_buffer_map_.try_emplace(depth_texture, depth_texture_buffer);

    return depth_texture_buffer;
}