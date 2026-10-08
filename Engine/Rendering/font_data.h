#pragma once
#include <directxtk12/SpriteFont.h>
#include <directxtk12/ResourceUploadBatch.h>

class DescriptorHandle;

namespace engine
{
/// <summary>
/// 文字の描画に使うフォントのデータを持つObjectです。
/// </summary>
class FontData : public Object
{
    static std::shared_ptr<DirectX::GraphicsMemory> m_graphics_memory_;
    static std::shared_ptr<DirectX::SpriteBatch> m_sprite_batch_;
    std::shared_ptr<DirectX::SpriteFont> m_sprite_font_;
    std::shared_ptr<DescriptorHandle> m_spritefont_handle_;

public:
    /// <summary>
    /// すべてのFontDataで共有するGraphicsMemoryを取得します。
    /// </summary>
    static std::shared_ptr<DirectX::GraphicsMemory> GraphicsMemory()
    {
        return m_graphics_memory_;
    }

    /// <summary>
    /// 文字の描画に使う、すべてのFontDataで共有するSpriteBatchを取得します。
    /// </summary>
    static std::shared_ptr<DirectX::SpriteBatch> SpriteBatch()
    {
        return m_sprite_batch_;
    }

    /// <summary>
    /// 読み込まれたSpriteFontを取得します。
    /// </summary>
    std::shared_ptr<DirectX::SpriteFont> SpriteFont()
    {
        return m_sprite_font_;
    }

    /// <summary>
    /// SpriteFontのTexture用に確保したDescriptorHandleを取得します。
    /// </summary>
    std::shared_ptr<DescriptorHandle> SpritefontHandle()
    {
        return m_spritefont_handle_;
    }

    /// <summary>
    /// 初めて生成された時に、共有のGraphicsMemoryとSpriteBatchを作成します。
    /// </summary>
    FontData();
    /// <summary>
    /// フォントファイル(.spritefont)からSpriteFontを読み込みます。
    /// </summary>
    /// <param name="font_path">フォントファイルのPath</param>
    void LoadFont(const std::wstring &font_path);
};
}