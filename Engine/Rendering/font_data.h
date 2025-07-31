#pragma once
#include <directxtk12/SpriteFont.h>
#include <directxtk12/ResourceUploadBatch.h>

class DescriptorHandle;

namespace engine
{
class FontData : public Object
{
    static std::shared_ptr<DirectX::GraphicsMemory> m_graphics_memory_;
    static std::shared_ptr<DirectX::SpriteBatch> m_sprite_batch_;
    std::shared_ptr<DirectX::SpriteFont> m_spritefont_;
    std::shared_ptr<DescriptorHandle> m_spritefont_handle_;

public:
    static std::shared_ptr<DirectX::GraphicsMemory> GraphicsMemory()
    {
        return m_graphics_memory_;
    }

    static std::shared_ptr<DirectX::SpriteBatch> SpriteBatch()
    {
        return m_sprite_batch_;
    }

    std::shared_ptr<DirectX::SpriteFont> SpriteFont()
    {
        return m_spritefont_;
    }

    std::shared_ptr<DescriptorHandle> SpritefontHandle()
    {
        return m_spritefont_handle_;
    }


    FontData();
    void LoadFont(const std::wstring &font_path);
};
}