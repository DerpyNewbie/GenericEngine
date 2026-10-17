#pragma once
#include <directxtk12/SpriteFont.h>
#include <directxtk12/ResourceUploadBatch.h>



namespace engine
{
class DescriptorHandle;

/// <summary>
/// SpriteFontを使ってテキストを描画するためのFontのデータです。SpriteBatchとGraphicsMemoryは全Fontで共有します。
/// </summary>
class FontData : public Object
{
    static std::shared_ptr<DirectX::GraphicsMemory> m_graphics_memory_;
    static std::shared_ptr<DirectX::SpriteBatch> m_sprite_batch_;
    std::shared_ptr<DirectX::SpriteFont> m_sprite_font_;
    std::shared_ptr<DescriptorHandle> m_spritefont_handle_;

public:
    /// <summary>
    /// 全Fontで共有するGraphicsMemoryを取得します。
    /// </summary>
    static std::shared_ptr<DirectX::GraphicsMemory> GraphicsMemory()
    {
        return m_graphics_memory_;
    }

    /// <summary>
    /// 全Fontで共有するSpriteBatchを取得します。
    /// </summary>
    static std::shared_ptr<DirectX::SpriteBatch> SpriteBatch()
    {
        return m_sprite_batch_;
    }

    /// <summary>
    /// 読み込んだSpriteFontを取得します。
    /// </summary>
    std::shared_ptr<DirectX::SpriteFont> SpriteFont()
    {
        return m_sprite_font_;
    }

    /// <summary>
    /// SpriteFontのTextureに割り当てたDescriptorHandleを取得します。
    /// </summary>
    std::shared_ptr<DescriptorHandle> SpritefontHandle()
    {
        return m_spritefont_handle_;
    }

    /// <summary>
    /// 初回のみ、共有のGraphicsMemoryとSpriteBatchを作成します。
    /// </summary>
    FontData();
    /// <summary>
    /// spritefontファイルを読み込み、TextureをDescriptorHeapに登録します。
    /// </summary>
    /// <param name="font_path">spritefontファイルのパス</param>
    void LoadFont(const std::wstring &font_path);
};
}