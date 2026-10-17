#pragma once
#include "depth_texture.h"
#include "depth_texture_buffer.h"
#include "render_texture.h"
#include "render_texture_buffer.h"
#include "texture_buffer.h"
#include "CabotEngine/Graphics/Texture2D.h"

namespace engine
{
/// <summary>
/// TextureとそのGPU用のTextureBufferの対応を管理するクラスです。同じTextureに対してBufferを1つだけ生成します。
/// </summary>
class TextureCollection
{
    friend class Texture2DImporter;
    friend class Texture2D;

    inline static std::unordered_map<std::shared_ptr<Texture2D>, std::shared_ptr<TextureBuffer>> m_textures_buffer_map_;
    inline static std::unordered_map<std::shared_ptr<RenderTexture>, std::shared_ptr<RenderTextureBuffer>> m_render_texture_buffer_map_;
    inline static std::unordered_map<std::shared_ptr<DepthTexture>, std::shared_ptr<DepthTextureBuffer>> m_depth_texture_buffer_map_;

    /// <summary>
    /// Textureの種類(RenderTexture/DepthTexture/通常)に応じたTextureBufferを生成して登録します。
    /// </summary>
    /// <param name="texture">対象のTexture</param>
    static std::shared_ptr<TextureBuffer> CreateTextureBuffer(const std::shared_ptr<Texture2D> &texture);

public:
    /// <summary>
    /// Textureに対応するTextureBufferを取得します。存在しない場合は生成します。
    /// </summary>
    /// <param name="texture">対象のTexture</param>
    static std::shared_ptr<TextureBuffer> GetTexture(const std::shared_ptr<Texture2D> &texture);
    /// <summary>
    /// RenderTextureに対応するRenderTextureBufferを取得します。存在しない場合は生成します。
    /// </summary>
    /// <param name="render_texture">対象のRenderTexture</param>
    static std::shared_ptr<RenderTextureBuffer> GetRenderTexture(const std::shared_ptr<RenderTexture> &render_texture);
    /// <summary>
    /// DepthTextureに対応するDepthTextureBufferを取得します。存在しない場合は生成します。
    /// </summary>
    /// <param name="depth_texture">対象のDepthTexture</param>
    static std::shared_ptr<DepthTextureBuffer> GetDepthTexture(std::shared_ptr<DepthTexture> depth_texture);

    /// <summary>
    /// Textureに対応するTextureBufferの登録を削除します。
    /// </summary>
    /// <param name="texture">対象のTexture</param>
    /// <returns>削除した場合はtrue</returns>
    static bool DeleteTexture(const std::shared_ptr<Texture2D> &texture);
    /// <summary>
    /// RenderTextureに対応するBufferの登録を削除します。
    /// </summary>
    /// <param name="render_texture">対象のRenderTexture</param>
    /// <returns>削除した場合はtrue</returns>
    static bool DeleteRenderTexture(const std::shared_ptr<RenderTexture> &render_texture);
    /// <summary>
    /// DepthTextureに対応するBufferの登録を削除します。
    /// </summary>
    /// <param name="depth_texture">対象のDepthTexture</param>
    /// <returns>削除した場合はtrue</returns>
    static bool DeleteDepthTexture(const std::shared_ptr<DepthTexture> &depth_texture);
};
}