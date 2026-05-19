#include "pch.h"
#include "texture_collection.h"

std::shared_ptr<engine::TextureBuffer> engine::TextureCollection::GetTexture(const std::shared_ptr<Texture2D> &texture)
{
    const auto it = m_textures_buffer_map_.find(texture);
    if (it != m_textures_buffer_map_.end())
        return it->second;

    auto texture_buffer = std::make_shared<TextureBuffer>(texture);
    m_textures_buffer_map_.try_emplace(texture, texture_buffer);

    return texture_buffer;
}

std::shared_ptr<engine::RenderTextureBuffer> engine::TextureCollection::GetRenderTexture(const std::shared_ptr<RenderTexture> &render_texture)
{
    const auto it = m_render_texture_buffer_map_.find(render_texture);
    if (it != m_render_texture_buffer_map_.end())
        return it->second;

    auto render_texture_buffer = std::make_shared<RenderTextureBuffer>(render_texture);
    m_textures_buffer_map_.try_emplace(render_texture, render_texture_buffer);
    m_render_texture_buffer_map_.try_emplace(render_texture, render_texture_buffer);

    return render_texture_buffer;
}

std::shared_ptr<engine::DepthTextureBuffer> engine::TextureCollection::GetDepthTexture(std::shared_ptr<DepthTexture> depth_texture)
{
    const auto it = m_depth_texture_buffer_map_.find(depth_texture);
    if (it != m_depth_texture_buffer_map_.end())
        return it->second;

    auto depth_texture_buffer = std::make_shared<DepthTextureBuffer>(depth_texture);
    m_textures_buffer_map_.try_emplace(depth_texture, depth_texture_buffer);
    m_depth_texture_buffer_map_.try_emplace(depth_texture, depth_texture_buffer);

    return depth_texture_buffer;
}

bool engine::TextureCollection::DeleteTexture(const std::shared_ptr<Texture2D> &texture)
{
    return m_textures_buffer_map_.erase(texture) == 1;
}

bool engine::TextureCollection::DeleteRenderTexture(const std::shared_ptr<RenderTexture> &render_texture)
{
    const auto erase_count = m_textures_buffer_map_.erase(render_texture) + m_render_texture_buffer_map_.erase(render_texture);

    return erase_count > 0;
}

bool engine::TextureCollection::DeleteDepthTexture(const std::shared_ptr<DepthTexture> &depth_texture)
{
    const auto erase_count = m_textures_buffer_map_.erase(depth_texture) + m_depth_texture_buffer_map_.erase(depth_texture);

    return erase_count > 0;
}